use syscall::error::*;
use syscall::scheme::Scheme;

use std::cmp;

pub struct ZeroScheme;

impl Scheme for ZeroScheme {
    fn open(&self, _path: &str, _flags: usize, _uid: u32, _gid: u32) -> Result<usize> {
        Ok(0)
    }

    fn dup(&self, _file: usize, buf: &[u8]) -> Result<usize> {
        if ! buf.is_empty() {
            return Err(Error::new(EINVAL));
        }

        Ok(0)
    }

    /// Read the file `number` into the `buffer`
    ///
    /// Returns the number of bytes read
    fn read(&self, _file: usize, buf: &mut [u8]) -> Result<usize> {
        let size = buf.len();

        buf[..size].iter_mut().for_each(|val| *val = 0);

        Ok(size)
    }

    /// Write the `buffer` to the `file`
    ///
    /// Returns the number of bytes written
    fn write(&self, _file: usize, buffer: &[u8]) -> Result<usize> {
        Ok(buffer.len())
    }

    fn fcntl(&self, _id: usize, _cmd: usize, _arg: usize) -> Result<usize> {
        Ok(0)
    }

    fn fpath(&self, _id: usize, buf: &mut [u8]) -> Result<usize> {
        let scheme_path = b"zero:";
        let size = cmp::min(buf.len(), scheme_path.len());

        buf[..size].copy_from_slice(&scheme_path[..size]);

        Ok(size)
    }

    fn fsync(&self, _file: usize) -> Result<usize> {
        Ok(0)
    }

    /// Close the file `number`
    fn close(&self, _file: usize) -> Result<usize> {
        Ok(0)
    }
}
