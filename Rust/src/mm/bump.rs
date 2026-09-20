//! A bump allocator just hands out memory from an increasing pointer and never
//! frees it. It's meant to be used as a throwaway allocator during early boot,
//! before a real allocator exists.

use core::ptr::write_bytes;

use crate::mm::PAGE_SIZE;

struct BumpAllocator {
    current: usize,
    limit: usize,
}

impl BumpAllocator {
    pub fn new(base_addr: usize, size: usize) -> Self {
        assert!(
            base_addr % PAGE_SIZE == 0,
            "provided addr must be page aligned"
        );

        Self {
            current: base_addr,
            limit: base_addr + size,
        }
    }

    pub fn alloc_pages(&mut self, count: usize) -> Option<usize> {
        let alloc_size = count * PAGE_SIZE;
        if self.current + alloc_size > self.limit {
            return None;
        }

        let alloc = self.current;
        self.current += alloc_size;

        // Zero it out since it will be used for paging structures.
        unsafe {
            write_bytes(alloc as *mut u8, 0, alloc_size);
        }

        return Some(alloc);
    }
}

#[cfg(test)]
mod tests {
    use crate::mm::test_util::TestRegion;

    use super::*;

    #[test]
    fn alloc() {
        let region = TestRegion::new(8 * PAGE_SIZE);
        let mut allocator = BumpAllocator::new(region.base_addr(), region.size());

        // alloc p1 | ***-----
        // alloc p2 | ******--
        // alloc p3 | *******-
        // alloc p4 | ********
        // alloc p5 | ******** - should fail

        let p1 = allocator.alloc_pages(3).expect("failed to alloc p1");
        assert_eq!(p1, region.base_addr());

        let p2 = allocator.alloc_pages(3).expect("failed to alloc p2");
        assert_eq!(p2, region.base_addr() + (3 * PAGE_SIZE));

        let p3 = allocator.alloc_pages(1).expect("failed to alloc p3");
        assert_eq!(p3, region.base_addr() + (6 * PAGE_SIZE));

        let p4 = allocator.alloc_pages(1).expect("failed to alloc p4");
        assert_eq!(p4, region.base_addr() + (7 * PAGE_SIZE));

        let p5 = allocator.alloc_pages(1);
        assert_eq!(p5, None);
    }

    #[test]
    #[should_panic(expected = "must be page aligned")]
    fn region_too_small_panics() {
        BumpAllocator::new(0x1337, 0x1337);
    }
}
