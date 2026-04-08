use core::arch::global_asm;

use crate::dbgprintln;

global_asm!(include_str!("handlers.asm"));

unsafe extern "C" {
    static isrs: [*const u8; 32];
}

#[unsafe(no_mangle)]
fn isr_handler() {}

pub fn init() {
    unsafe {
        dbgprintln!("Handlers: {:p}", &isrs);
    }
}
