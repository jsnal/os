use crate::{dbgprintln, linker_sym__end};

mod buddy;
mod bump;
#[cfg(test)]
mod test_util;

// Standard page size on x86.
pub const PAGE_SIZE: usize = 4096;

// #[repr(transparent)]
// #[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Debug)]
// pub struct PhysAddr(pub usize);
//
// #[repr(transparent)]
// #[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Debug)]
// pub struct VirtAddr(pub usize);

pub fn early_init() {
    // TODO: Make sure this memory is actually available by checking the
    // multiboot info.
    dbgprintln!("__end={:#x}", linker_sym__end());

    bump::BumpAllocator::new(linker_sym__end(), 16);
}
