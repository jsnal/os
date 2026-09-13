use core::{ops::DerefMut, ptr::NonNull};

use crate::mm::{PAGE_SIZE, PhysAddr};
use crate::{dbgprint, dbgprintln};

const BLK_SIZE: usize = PAGE_SIZE;
const MAX_ORDER: usize = 10;

#[repr(C)]
struct BlockInfo {
    order: u8,
    free: bool,
}

#[repr(C)]
struct FreeBlock {
    next: Option<NonNull<FreeBlock>>,
    prev: Option<NonNull<FreeBlock>>,
}

struct BuddyAllocator {
    base_addr: usize,
    total_blks: usize,
    usable_start_blk_idx: usize,
    info: *mut BlockInfo,
    free_lists: [Option<NonNull<FreeBlock>>; MAX_ORDER + 1],
}

impl BuddyAllocator {
    pub fn new(base_addr: usize, size: usize) -> Self {
        assert!(
            base_addr % BLK_SIZE == 0,
            "memory region base must be block-aligned"
        );
        assert!(size >= BLK_SIZE, "memory region is too small");

        let total_blks = size / BLK_SIZE;
        let info_size = total_blks * size_of::<BlockInfo>();
        let info_blks = info_size.div_ceil(BLK_SIZE);
        let usable_blks = total_blks - info_blks;
        assert!(
            info_blks < total_blks,
            "memory region is too small to hold block info"
        );

        // Initialize the info list that lives at the start of the memory region.
        let info = base_addr as *mut BlockInfo;
        for i in 0..usable_blks {
            unsafe {
                core::ptr::write(
                    info.add(i),
                    BlockInfo {
                        order: 0,
                        free: false,
                    },
                );
            }
        }

        let mut allocator = Self {
            base_addr,
            total_blks,
            usable_start_blk_idx: info_blks,
            info,
            free_lists: [None; MAX_ORDER + 1],
        };

        // Initialize the free list with the maximum number of power-of-two blocks that will fit.
        let mut blk_idx = 0;
        while blk_idx < usable_blks {
            let blks_left = usable_blks - blk_idx;
            let max_by_size = blks_left.ilog2() as usize;
            let max_by_align = if blk_idx == 0 {
                MAX_ORDER
            } else {
                blks_left.trailing_zeros() as usize
            };
            let order = max_by_size.min(max_by_align).min(MAX_ORDER);
            allocator.push_free_blk(order as u8, blk_idx);
            blk_idx += 1 << order;
        }

        allocator
    }

    pub fn alloc(&mut self, order: u8) -> Option<usize> {
        assert!(order as usize <= MAX_ORDER, "provided order is too large");

        // Find the index of the smallest possible free block.
        let mut free_order = order;
        let blk_idx = loop {
            if let Some(blk_idx) = self.pop_free_blk(free_order) {
                break blk_idx;
            }
            if (free_order as usize) >= MAX_ORDER {
                return None;
            }
            free_order += 1;
        };

        // Continuously split blocks until the smallest possible order that will fit.
        while free_order > order {
            free_order -= 1;
            let buddy_blk_idx = blk_idx + (1 << free_order);
            self.push_free_blk(free_order, buddy_blk_idx);
        }

        // Mark the block as taken in its own info.
        let info = unsafe { &mut *self.info.add(blk_idx) };
        info.order = order;
        info.free = false;

        Some(self.blk_idx_to_addr(blk_idx))
    }

    pub fn free(&mut self, addr: usize, order: u8) {
        let mut blk_idx = self.addr_to_blk_idx(addr);
        assert!(
            blk_idx >= self.usable_start_blk_idx,
            "provided addr is outside of the memory region"
        );
        assert!(
            blk_idx < self.total_blks,
            "provided addr is outside of the memory region"
        );
        assert!(
            !(unsafe { &mut *self.info.add(blk_idx) }).free,
            "double free"
        );

        // Continuously coalesce blocks as long as their buddy is free.
        let mut new_order = order;
        while (new_order as usize) < MAX_ORDER {
            let buddy_blk_idx = blk_idx ^ (1 << new_order);
            let buddy_info = unsafe { &mut *self.info.add(blk_idx) };

            if buddy_blk_idx >= self.total_blks || !buddy_info.free || buddy_info.order != new_order
            {
                break;
            }

            self.remove_free_blk(new_order, buddy_blk_idx);
            blk_idx = blk_idx.min(buddy_blk_idx);
            new_order += 1;
        }

        self.push_free_blk(new_order, blk_idx);
    }

    fn pages_to_order(&self, count: usize) -> u8 {
        let count = count.max(1);
        if count == 1 {
            0
        } else {
            (usize::BITS - (count - 1).leading_zeros()) as u8
        }
    }

    pub fn alloc_pages(&mut self, count: usize) -> Option<usize> {
        self.alloc(self.pages_to_order(count))
    }

    pub fn free_pages(&mut self, addr: usize, count: usize) {
        self.free(addr, self.pages_to_order(count))
    }

    fn blk_idx_to_addr(&self, blk_idx: usize) -> usize {
        self.base_addr + (blk_idx * BLK_SIZE)
    }

    fn addr_to_blk_idx(&self, addr: usize) -> usize {
        (addr - self.base_addr) / BLK_SIZE
    }

    fn push_free_blk(&mut self, order: u8, blk_idx: usize) {
        // Push the free block to the head of that order's list.
        let blk_ptr = self.blk_idx_to_addr(blk_idx) as *mut FreeBlock;
        let head_opt = self.free_lists[order as usize];
        unsafe {
            (*blk_ptr).prev = None;
            (*blk_ptr).next = head_opt;
            if let Some(mut head) = head_opt {
                head.as_mut().prev = NonNull::new(blk_ptr);
            }
        }
        self.free_lists[order as usize] = NonNull::new(blk_ptr);

        // Mark the block as free in its own info.
        let info = unsafe { &mut *self.info.add(blk_idx) };
        info.order = order;
        info.free = true;
    }

    fn remove_free_blk(&mut self, order: u8, blk_idx: usize) {
        // Remove the free block and update neighboring pointers.
        let blk_ptr = self.blk_idx_to_addr(blk_idx) as *mut FreeBlock;
        unsafe {
            if let Some(mut prev) = (*blk_ptr).prev {
                prev.as_mut().next = (*blk_ptr).next;
            } else {
                self.free_lists[order as usize] = (*blk_ptr).next;
            }

            if let Some(mut next) = (*blk_ptr).next {
                next.as_mut().prev = (*blk_ptr).prev;
            }
        }

        // Mark the block as taken in its own info.
        let info = unsafe { &mut *self.info.add(blk_idx) };
        info.free = false;
    }

    fn pop_free_blk(&mut self, order: u8) -> Option<usize> {
        let head = self.free_lists[order as usize]?;
        let blk_idx = self.addr_to_blk_idx(head.as_ptr() as usize);
        self.remove_free_blk(order, blk_idx);
        Some(blk_idx)
    }
}

#[cfg(test)]
mod tests {
    use core::alloc::Layout;

    use super::*;

    struct TestRegion {
        ptr: *mut u8,
        layout: Layout,
    }

    impl TestRegion {
        fn new(size: usize) -> Self {
            let layout = Layout::from_size_align(size, BLK_SIZE).expect("invalid layout");
            let ptr = unsafe { std::alloc::alloc_zeroed(layout) };
            assert!(!ptr.is_null(), "allocation failed");
            Self { ptr, layout }
        }

        fn as_ptr(&self) -> *mut u8 {
            self.ptr
        }
    }

    impl Drop for TestRegion {
        fn drop(&mut self) {
            unsafe { std::alloc::dealloc(self.ptr, self.layout) };
        }
    }

    #[test]
    fn basic_alloc_and_free() {
        let region = TestRegion::new(16 * BLK_SIZE);
        let mut allocator = BuddyAllocator::new(region.as_ptr() as usize, region.layout.size());

        println!("region={:x}", region.as_ptr() as usize);

        let p1 = allocator.alloc_pages(2);
        println!("p1={:x}", p1.unwrap());

        assert_eq!(true, false);
    }

    #[test]
    #[should_panic(expected = "block-aligned")]
    fn misaligned_base_panics() {
        let region = TestRegion::new(16 * BLK_SIZE);
        BuddyAllocator::new(region.as_ptr() as usize + 10, region.layout.size());
    }

    #[test]
    #[should_panic(expected = "too small")]
    fn region_too_small_panics() {
        let region = TestRegion::new(BLK_SIZE - 10);
        BuddyAllocator::new(region.as_ptr() as usize, region.layout.size());
    }
}
