#![cfg_attr(not(test), no_std)]

#[cfg(not(test))]
pub mod boot;
#[cfg(not(test))]
pub mod cpu;

pub mod debug;
pub mod mm;

#[inline(always)]
#[allow(non_snake_case)]
pub fn linker_sym__end() -> usize {
    unsafe extern "C" {
        static __end: u8;
    }
    (&raw const __end) as usize
}
