mod buddy;
mod bump;
#[cfg(test)]
mod test_util;

#[repr(transparent)]
#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Debug)]
pub struct PhysAddr(pub usize);

#[repr(transparent)]
#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Debug)]
pub struct VirtAddr(pub usize);

// Standard page size on x86.
const PAGE_SIZE: usize = 4096;
