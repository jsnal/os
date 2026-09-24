use core::arch::asm;

pub mod gdt;
pub mod idt;
pub mod io;
pub mod pic;
pub mod pit;

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

pub fn init() {
    gdt::init();
    idt::init();
    pic::init();
    pit::init();
}
