#![no_std]
#![no_main]

use core::{arch::global_asm, panic::PanicInfo};
use kernel::{boot::multiboot, cpu, dbgprintln, mm};

global_asm!(include_str!("boot/boot.s"));

static HELLO: &[u8] = b"Hello World!";

// const multiboot_information_t* multiboot, const u32 magic_number
#[unsafe(no_mangle)]
pub extern "C" fn kmain(mb: &multiboot::Info, magic_number: u32) -> ! {
    cpu::init();

    let vga_buffer = 0xb8000 as *mut u8;
    for (i, &byte) in HELLO.iter().enumerate() {
        unsafe {
            *vga_buffer.offset(i as isize * 2) = byte;
            *vga_buffer.offset(i as isize * 2 + 1) = 0xb;
        }
    }

    if magic_number != multiboot::BOOTLOADER_MAGIC {
        panic!("invalid multiboot header magic number");
    }

    dbgprintln!("multiboot: magic_number={:#x}", magic_number);
    let mem_lower = mb.mem_lower;
    let mem_upper = mb.mem_upper;
    dbgprintln!(
        "multiboot: mem_lower={:#x}, mem_upper={:#x}",
        mem_lower,
        mem_upper
    );

    mm::early_init();

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
