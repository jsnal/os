use core::arch::asm;

pub mod gdt;
pub mod idt;
pub mod io;

#[inline]
pub fn cli() {
    unsafe {
        asm!("cli");
    }
}

#[inline]
pub fn hlt() {
    unsafe {
        asm!("hlt");
    }
}
