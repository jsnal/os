use core::fmt;

pub struct DebugConsole {
    port: u16,
}

impl DebugConsole {
    pub const fn new() -> Self {
        Self { port: 0xE9 }
    }

    pub fn write_byte(&self, byte: u8) {
        #[cfg(not(test))]
        crate::cpu::io::outb(self.port, byte);

        // Test builds have no port I/O so write to stdout instead so `dbgprint!` / `dbgprintln!`
        // output shows up in test runs.
        #[cfg(test)]
        {
            use std::io::Write;
            let _ = self.port;
            let _ = std::io::stdout().write_all(&[byte]);
        }
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
    ($($args:tt)*) => {
        {
            use core::fmt::Write;
            let mut debug = $crate::debug::DebugConsole::new();
            let _ = write!(debug, $($args)*);
        }
    };
}

#[macro_export]
macro_rules! dbgprintln {
    () => ($crate::dbgprint!("\n"));
    ($fmt:expr) => ($crate::dbgprint!(concat!($fmt, "\n")));
    ($fmt:expr, $($args:tt)*) => ($crate::dbgprint!(
        concat!($fmt, "\n"), $($args)*));
}
