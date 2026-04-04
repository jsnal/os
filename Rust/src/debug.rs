use core::fmt;

use crate::cpu::io;

pub struct DebugConsole {
    port: u16,
}

impl DebugConsole {
    pub const fn new() -> Self {
        Self { port: 0xE9 }
    }

    pub fn write_byte(&self, byte: u8) {
        io::outb(self.port, byte);
    }

    pub fn write_string(&self, s: &str) {
        for byte in s.bytes() {
            self.write_byte(byte);
        }
    }
}

impl fmt::Write for DebugConsole {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        self.write_string(s);
        Ok(())
    }
}

#[macro_export]
macro_rules! dbgprint {
    ($($arg:tt)*) => {
        {
            use core::fmt::Write;
            let mut debug = $crate::debug::DebugConsole::new();
            let _ = write!(debug, $($arg)*);
        }
    };
}
