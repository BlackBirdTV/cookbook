#![feature(asm_const)]

#[macro_use]
extern crate bitflags;
extern crate orbclient;
extern crate syscall;

use std::{env, process};
use std::fs::{File, OpenOptions};
use std::io::{Read, Write};
use std::os::unix::fs::OpenOptionsExt;
use std::os::unix::io::AsRawFd;

use syscall::call::iopl;

use crate::state::Ps2d;

mod controller;
mod keymap;
mod state;
mod vm;

fn daemon(daemon: syscall::Daemon) -> core::convert::Infallible {
    unsafe {
        iopl(3).expect("ps2d: failed to get I/O permission");
    }

    let (keymap, keymap_name): (
        fn(u8, bool) -> char,
        &str
    ) = match env::args().skip(1).next() {
        Some(k) => match k.to_lowercase().as_ref() {
            "dvorak" => (keymap::dvorak::get_char, "dvorak"),
            "us" => (keymap::us::get_char, "us"),
            "gb" => (keymap::gb::get_char, "gb"),
            "azerty" => (keymap::azerty::get_char, "azerty"),
            "bepo" => (keymap::bepo::get_char, "bepo"),
            "it" => (keymap::it::get_char, "it"),
            &_ => (keymap::us::get_char, "us"),
        },
        None => (keymap::us::get_char, "us"),
    };

    eprintln!("ps2d: using keymap '{}'", keymap_name);

    let input = OpenOptions::new()
        .write(true)
        .open("display:input")
        .expect("ps2d: failed to open display:input");

    let mut event_file = OpenOptions::new()
        .read(true)
        .write(true)
        .open("event:")
        .expect("ps2d: failed to open event:");

    let mut key_file = OpenOptions::new()
        .read(true)
        .write(true)
        .custom_flags(syscall::O_NONBLOCK as i32)
        .open("serio:0")
        .expect("ps2d: failed to open serio:0");

    event_file.write(&syscall::Event {
        id: key_file.as_raw_fd() as usize,
        flags: syscall::EVENT_READ,
        data: 0
    }).expect("ps2d: failed to event serio:0");

    let mut mouse_file = OpenOptions::new()
        .read(true)
        .write(true)
        .custom_flags(syscall::O_NONBLOCK as i32)
        .open("serio:1")
        .expect("ps2d: failed to open serio:1");

    event_file.write(&syscall::Event {
        id: mouse_file.as_raw_fd() as usize,
        flags: syscall::EVENT_READ,
        data: 1
    }).expect("ps2d: failed to event irq:12");

    syscall::setrens(0, 0).expect("ps2d: failed to enter null namespace");

    daemon.ready().expect("ps2d: failed to mark daemon as ready");

    let mut ps2d = Ps2d::new(input, keymap);

    let mut data = [0; 256];
    loop {
        // There are some gotchas with ps/2 controllers that require this weird
        // way of doing things. You read key and mouse data from the same
        // place. There is a status register that may show you which the data
        // came from, but if it is even implemented it can have a race
        // condition causing keyboard data to be read as mouse data.
        //
        // Due to this, we have a kernel driver doing a small amount of work
        // to grab bytes and sort them based on the source

        let mut event = syscall::Event::default();
        if event_file.read(&mut event).expect("ps2d: failed to read event file") == 0 {
            break;
        }

        let (file, keyboard) = match event.data {
            0 => (&mut key_file, true),
            1 => (&mut mouse_file, false),
            _ => continue,
        };

        loop {
            let count = match file.read(&mut data) {
                Ok(0) => break,
                Ok(count) => count,
                Err(_) => break,
            };
            for i in 0..count {
                ps2d.handle(keyboard, data[i]);
            }
        }
    }

    process::exit(0);
}

fn main() {
    syscall::Daemon::new(daemon).expect("ps2d: failed to create daemon");
}
