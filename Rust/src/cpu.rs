use core::arch::asm;

pub mod gdt;
pub mod idt;
pub mod io;
pub mod pic;

#[inline]
pub fn cli() {
    unsafe {
        asm!("cli");
    }
}

#[inline]
pub fn sti() {
    unsafe {
        asm!("sti");
    }
}

#[inline]
pub fn hlt() {
    unsafe {
        asm!("hlt");
    }
}
