use std::convert::TryInto;
use std::fs::File;
use std::io::{ErrorKind, Read, Write};
use std::os::unix::io::{FromRawFd, RawFd};
use std::ptr::NonNull;
use std::sync::{Arc, Mutex};
use std::{slice, usize};

use pcid_interface::{PciBar, PciFeature, PciFeatureInfo, PciFunction, PcidServerHandle};
use syscall::{
    CloneFlags, Event, Mmio, Packet, Result, SchemeBlockMut, PHYSMAP_NO_CACHE,
    PHYSMAP_WRITE,
};
use redox_log::{OutputBuilder, RedoxLogger};

use self::nvme::{InterruptMethod, InterruptSources, Nvme};
use self::scheme::DiskScheme;

mod nvme;
mod scheme;

/// A wrapper for a BAR allocation.
pub struct Bar {
    ptr: NonNull<u8>,
    physical: usize,
    bar_size: usize,
}
impl Bar {
    pub fn allocate(bar: usize, bar_size: usize) -> Result<Self> {
        Ok(Self {
            ptr: NonNull::new(
                unsafe { syscall::physmap(bar, bar_size, PHYSMAP_NO_CACHE | PHYSMAP_WRITE)? as *mut u8 },
            )
            .expect("Mapping a BAR resulted in a nullptr"),
            physical: bar,
            bar_size,
        })
    }
}

impl Drop for Bar {
    fn drop(&mut self) {
        let _ = unsafe { syscall::physunmap(self.physical) };
    }
}

/// The PCI BARs that may be allocated.
#[derive(Default)]
pub struct AllocatedBars(pub [Mutex<Option<Bar>>; 6]);

/// Get the most optimal yet functional interrupt mechanism: either (in the order of preference):
/// MSI-X, MSI, and INTx# pin. Returns both runtime interrupt structures (MSI/MSI-X capability
/// structures), and the handles to the interrupts.
fn get_int_method(
    pcid_handle: &mut PcidServerHandle,
    function: &PciFunction,
    allocated_bars: &AllocatedBars,
) -> Result<(InterruptMethod, InterruptSources)> {
    log::trace!("Begin get_int_method");
    use pcid_interface::irq_helpers;

    let features = pcid_handle.fetch_all_features().unwrap();

    let has_msi = features.iter().any(|(feature, _)| feature.is_msi());
    let has_msix = features.iter().any(|(feature, _)| feature.is_msix());

    // TODO: Allocate more than one vector when possible and useful.
    if has_msix {
        // Extended message signaled interrupts.
        use self::nvme::MsixCfg;
        use pcid_interface::msi::MsixTableEntry;

        let mut capability_struct = match pcid_handle.feature_info(PciFeature::MsiX).unwrap() {
            PciFeatureInfo::MsiX(msix) => msix,
            _ => unreachable!(),
        };
        fn bar_base(
            allocated_bars: &AllocatedBars,
            function: &PciFunction,
            bir: u8,
        ) -> Result<NonNull<u8>> {
            let bir = usize::from(bir);
            let mut bar_guard = allocated_bars.0[bir].lock().unwrap();
            match &mut *bar_guard {
                &mut Some(ref bar) => Ok(bar.ptr),
                bar_to_set @ &mut None => {
                    let bar = match function.bars[bir] {
                        PciBar::Memory(addr) => match addr {
                            0 => panic!("BAR {} is mapped to address 0", bir),
                            _ => addr,
                        },
                        other => panic!("Expected memory BAR, found {:?}", other),
                    };
                    let bar_size = function.bar_sizes[bir];

                    let bar = Bar::allocate(bar as usize, bar_size as usize)?;
                    *bar_to_set = Some(bar);
                    Ok(bar_to_set.as_ref().unwrap().ptr)
                }
            }
        }
        let table_bar_base: *mut u8 =
            bar_base(allocated_bars, function, capability_struct.table_bir())?.as_ptr();
        let pba_bar_base: *mut u8 =
            bar_base(allocated_bars, function, capability_struct.pba_bir())?.as_ptr();
        let table_base =
            unsafe { table_bar_base.offset(capability_struct.table_offset() as isize) };
        let pba_base = unsafe { pba_bar_base.offset(capability_struct.pba_offset() as isize) };

        let vector_count = capability_struct.table_size();
        let table_entries: &'static mut [MsixTableEntry] = unsafe {
            slice::from_raw_parts_mut(table_base as *mut MsixTableEntry, vector_count as usize)
        };
        let pba_entries: &'static mut [Mmio<u64>] = unsafe {
            slice::from_raw_parts_mut(
                table_base as *mut Mmio<u64>,
                (vector_count as usize + 63) / 64,
            )
        };

        // Mask all interrupts in case some earlier driver/os already unmasked them (according to
        // the PCI Local Bus spec 3.0, they are masked after system reset).
        for table_entry in table_entries.iter_mut() {
            table_entry.mask();
        }

        pcid_handle.enable_feature(PciFeature::MsiX).unwrap();
        capability_struct.set_msix_enabled(true); // only affects our local mirror of the cap

        let (msix_vector_number, irq_handle) = {
            use msi_x86_64::DeliveryMode;
            use pcid_interface::msi::x86_64 as msi_x86_64;

            let entry: &mut MsixTableEntry = &mut table_entries[0];

            let bsp_cpu_id =
                irq_helpers::read_bsp_apic_id().expect("nvmed: failed to read APIC ID");
            let bsp_lapic_id = bsp_cpu_id
                .try_into()
                .expect("nvmed: BSP local apic ID couldn't fit inside u8");
            let (vector, irq_handle) = irq_helpers::allocate_single_interrupt_vector(bsp_cpu_id)
                .expect("nvmed: failed to allocate single MSI-X interrupt vector")
                .expect("nvmed: no interrupt vectors left on BSP");

            let msg_addr = msi_x86_64::message_address(bsp_lapic_id, false, false);
            let msg_data = msi_x86_64::message_data_edge_triggered(DeliveryMode::Fixed, vector);

            entry.set_addr_lo(msg_addr);
            entry.set_msg_data(msg_data);

            (0, irq_handle)
        };

        let interrupt_method = InterruptMethod::MsiX(MsixCfg {
            cap: capability_struct,
            table: table_entries,
            pba: pba_entries,
        });
        let interrupt_sources =
            InterruptSources::MsiX(std::iter::once((msix_vector_number, irq_handle)).collect());

        Ok((interrupt_method, interrupt_sources))
    } else if has_msi {
        // Message signaled interrupts.
        let capability_struct = match pcid_handle.feature_info(PciFeature::Msi).unwrap() {
            PciFeatureInfo::Msi(msi) => msi,
            _ => unreachable!(),
        };

        let (msi_vector_number, irq_handle) = {
            use msi_x86_64::DeliveryMode;
            use pcid_interface::msi::x86_64 as msi_x86_64;
            use pcid_interface::{MsiSetFeatureInfo, SetFeatureInfo};

            let bsp_cpu_id =
                irq_helpers::read_bsp_apic_id().expect("nvmed: failed to read BSP APIC ID");
            let bsp_lapic_id = bsp_cpu_id
                .try_into()
                .expect("nvmed: BSP local apic ID couldn't fit inside u8");
            let (vector, irq_handle) = irq_helpers::allocate_single_interrupt_vector(bsp_cpu_id)
                .expect("nvmed: failed to allocate single MSI interrupt vector")
                .expect("nvmed: no interrupt vectors left on BSP");

            let msg_addr = msi_x86_64::message_address(bsp_lapic_id, false, false);
            let msg_data =
                msi_x86_64::message_data_edge_triggered(DeliveryMode::Fixed, vector) as u16;

            pcid_handle.set_feature_info(SetFeatureInfo::Msi(MsiSetFeatureInfo {
                message_address: Some(msg_addr),
                message_upper_address: Some(0),
                message_data: Some(msg_data),
                multi_message_enable: Some(0), // enable 2^0=1 vectors
                mask_bits: None,
            })).unwrap();

            (0, irq_handle)
        };

        let interrupt_method = InterruptMethod::Msi(capability_struct);
        let interrupt_sources =
            InterruptSources::Msi(std::iter::once((msi_vector_number, irq_handle)).collect());

        pcid_handle.enable_feature(PciFeature::Msi).unwrap();

        Ok((interrupt_method, interrupt_sources))
    } else if function.legacy_interrupt_pin.is_some() {
        // INTx# pin based interrupts.
        let irq_handle = File::open(format!("irq:{}", function.legacy_interrupt_line))
            .expect("nvmed: failed to open INTx# interrupt line");
        Ok((InterruptMethod::Intx, InterruptSources::Intx(irq_handle)))
    } else {
        // No interrupts at all
        todo!("handling of no interrupts")
    }
}

fn setup_logging() -> Option<&'static RedoxLogger> {
    let mut logger = RedoxLogger::new()
        .with_output(
            OutputBuilder::stderr()
                .with_filter(log::LevelFilter::Debug) // limit global output to important info
                .with_ansi_escape_codes()
                .flush_on_newline(true)
                .build()
        );

    #[cfg(target_os = "redox")]
    match OutputBuilder::in_redox_logging_scheme("disk", "pcie", "nvme.log") {
        Ok(b) => logger = logger.with_output(
            // TODO: Add a configuration file for this
            b.with_filter(log::LevelFilter::Info)
                .flush_on_newline(true)
                .build()
        ),
        Err(error) => eprintln!("nvmed: failed to create nvme.log: {}", error),
    }

    #[cfg(target_os = "redox")]
    match OutputBuilder::in_redox_logging_scheme("disk", "pcie", "nvme.ansi.log") {
        Ok(b) => logger = logger.with_output(
            b.with_filter(log::LevelFilter::Info)
                .with_ansi_escape_codes()
                .flush_on_newline(true)
                .build()
        ),
        Err(error) => eprintln!("nvmed: failed to create nvme.ansi.log: {}", error),
    }

    match logger.enable() {
        Ok(logger_ref) => {
            eprintln!("nvmed: enabled logger");
            Some(logger_ref)
        }
        Err(error) => {
            eprintln!("nvmed: failed to set default logger: {}", error);
            None
        }
    }
}

fn main() {
    // Daemonize
    if unsafe { syscall::clone(CloneFlags::empty()).unwrap() } != 0 {
        return;
    }

    let _logger_ref = setup_logging();

    let mut pcid_handle =
        PcidServerHandle::connect_default().expect("nvmed: failed to setup channel to pcid");
    let pci_config = pcid_handle
        .fetch_config()
        .expect("nvmed: failed to fetch config");

    let bar = match pci_config.func.bars[0] {
        PciBar::Memory(mem) => match mem {
            0 => panic!("BAR 0 is mapped to address 0"),
            _ => mem,
        },
        other => panic!("received a non-memory BAR ({:?})", other),
    };
    let bar_size = pci_config.func.bar_sizes[0];
    let irq = pci_config.func.legacy_interrupt_line;

    let mut name = pci_config.func.name();
    name.push_str("_nvme");

    log::info!("NVME PCI CONFIG: {:?}", pci_config);

    let allocated_bars = AllocatedBars::default();

    let address = unsafe {
        syscall::physmap(
            bar as usize,
            bar_size as usize,
            PHYSMAP_WRITE | PHYSMAP_NO_CACHE,
        )
        .expect("nvmed: failed to map address")
    };
    *allocated_bars.0[0].lock().unwrap() = Some(Bar {
        physical: bar as usize,
        bar_size: bar_size as usize,
        ptr: NonNull::new(address as *mut u8).expect("Physmapping BAR gave nullptr"),
    });
    let event_fd = syscall::open("event:", syscall::O_RDWR | syscall::O_CLOEXEC)
        .expect("nvmed: failed to open event queue");
    let mut event_file = unsafe { File::from_raw_fd(event_fd as RawFd) };

    let scheme_name = format!("disk/{}", name);
    let socket_fd = syscall::open(
        &format!(":{}", scheme_name),
        syscall::O_RDWR | syscall::O_CREAT | syscall::O_NONBLOCK | syscall::O_CLOEXEC,
    )
    .expect("nvmed: failed to create disk scheme");

    syscall::write(
        event_fd,
        &syscall::Event {
            id: socket_fd,
            flags: syscall::EVENT_READ,
            data: 0,
        },
    )
    .expect("nvmed: failed to watch disk scheme events");

    let mut socket_file = unsafe { File::from_raw_fd(socket_fd as RawFd) };

    let (reactor_sender, reactor_receiver) = crossbeam_channel::unbounded();
    let (interrupt_method, interrupt_sources) =
        get_int_method(&mut pcid_handle, &pci_config.func, &allocated_bars)
            .expect("nvmed: failed to find a suitable interrupt method");
    let mut nvme = Nvme::new(address, interrupt_method, pcid_handle, reactor_sender)
        .expect("nvmed: failed to allocate driver data");
    unsafe { nvme.init() }
    log::debug!("Finished base initialization");
    let nvme = Arc::new(nvme);
    let reactor_thread = nvme::cq_reactor::start_cq_reactor_thread(Arc::clone(&nvme), interrupt_sources, reactor_receiver);
    let namespaces = nvme.init_with_queues();

    syscall::setrens(0, 0).expect("nvmed: failed to enter null namespace");

    let mut scheme = DiskScheme::new(scheme_name, nvme, namespaces);
    let mut todo = Vec::new();
    'events: loop {
        let mut event = Event::default();
        if event_file
            .read(&mut event)
            .expect("nvmed: failed to read event queue")
            == 0
        {
            break;
        }

        match event.data {
            0 => loop {
                let mut packet = Packet::default();
                match socket_file.read(&mut packet) {
                    Ok(0) => break 'events,
                    Ok(_) => (),
                    Err(err) => match err.kind() {
                        ErrorKind::WouldBlock => break,
                        _ => Err(err).expect("nvmed: failed to read disk scheme"),
                    },
                }
                todo.push(packet);
            },
            unknown => {
                panic!("nvmed: unknown event data {}", unknown);
            }
        }

        let mut i = 0;
        while i < todo.len() {
            if let Some(a) = scheme.handle(&todo[i]) {
                let mut packet = todo.remove(i);
                packet.a = a;
                socket_file
                    .write(&packet)
                    .expect("nvmed: failed to write disk scheme");
            } else {
                i += 1;
            }
        }
    }

    //TODO: destroy NVMe stuff
    reactor_thread.join().expect("nvmed: failed to join reactor thread");
}
