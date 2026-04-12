#![no_std]
#![no_main]

use core::{arch::global_asm, panic::PanicInfo};
use kernel::{cpu, dbgprintln};

global_asm!(include_str!("boot/boot.s"));

static HELLO: &[u8] = b"Hello World!";

#[unsafe(no_mangle)]
pub extern "C" fn kmain() -> ! {
    cpu::init();

    let vga_buffer = 0xb8000 as *mut u8;
    for (i, &byte) in HELLO.iter().enumerate() {
        unsafe {
            *vga_buffer.offset(i as isize * 2) = byte;
            *vga_buffer.offset(i as isize * 2 + 1) = 0xb;
        }
    }

    loop {
        cpu::hlt();
    }
}

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    dbgprintln!(
        "kernel panic at {}: {}",
        info.location().unwrap(),
        info.message()
    );

    cpu::cli();
    loop {
        cpu::hlt();
    }
}
