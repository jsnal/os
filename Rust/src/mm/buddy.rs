//! A binary buddy allocator is a memory allocation technique where every block
//! is a power-of-two multiple of some base block size. This makes splitting
//! and merging cheap and keeps fragmentation low over long usage.
//!
//! # The "buddy"
//! A block of "order `k`" is `2^k` blocks large. Every order-`k` block  was
//! produced by splitting exactly one order-`(k + 1)` block in half, and the two
//! halves are each other's "buddy" for as long as both stay free. Because of
//! how they were split, a block and its buddy are always adjacent in memory and
//! their positions differ in exactly one bit. This bit corresponds to their
//! order making it easy to compute the other's location directly with no
//! separate bookkeeping.
//!
//! # Allocating
//! A request for order `k` is returned straight from a free list of order-`k`
//! blocks if one exists. Otherwise, the allocator takes the smallest
//! available block of some larger order and repeatedly splits it in half,
//! keeping one half and pushing the other, which is now a free buddy, onto its
//! own order's free list, until it has shrunk a block down to the requested
//! order.
//!
//! # Freeing
//! Freeing does the exact opposite procedure as allocating. A freed block's
//! buddy is checked and if that buddy is also entirely free, the two are merged
//! back into a single block of the next order up. This repeats with the new
//! block's buddy until one is found still in use, or the largest
//! order is reached. This chain of merges is what keeps the allocator from
//! fragmenting since memory freed in the right order always finds its way back
//! into large, contiguous blocks.
//!
//! # Trade-offs
//! Rounding every request up to a power-of-two block wastes up to nearly
//! half a block per allocation to internal fragmentation, and two free
//! neighbors that aren't buddies can never be merged, even if doing so would
//! produce a useful contiguous block. That's the price paid for buddy
//! lookups and split/merge chains that are cheap and bounded by the number
//! of orders, instead of scanning a large free list.
//!
//! # Limitations
//! The free list is stored as an intrusive linked list, meaning that the next
//! and prev pointers are stored at the beginning of each free block. This lets
//! us avoid allocating a separate data structure for tracking free blocks, but
//! means the entire memory region must fall inside the kernel's physical direct
//! map. All blocks must be be dereferenceable just to link or unlink it
//! from a free list. This could be fixed by making the free list aware of this
//! possibility and temporarily mapping pages outside of the region so they can
//! be edited. Usually this is done in a dedicated region at the top of the
//! kernel's virtual memory space.
//!
//! # Resources
//! - https://wiki.osdev.org/Page_Frame_Allocation
//! - https://www.kernel.org/doc/gorman/html/understand/understand009.html

use core::ptr::NonNull;

use crate::mm::PAGE_SIZE;
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
    total_blks: usize,
    alloc_area_addr: usize,
    alloc_area_blk_idx: usize,
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

        // Initialize the info list that lives at the start of the memory
        // region.
        let info = base_addr as *mut BlockInfo;
        for i in 0..usable_blks {
            let info = unsafe { &mut *info.add(i) };
            info.order = 0;
            info.free = false;
        }

        let mut allocator = Self {
            total_blks,
            alloc_area_addr: base_addr + (info_blks * BLK_SIZE),
            alloc_area_blk_idx: info_blks,
            info,
            free_lists: [None; MAX_ORDER + 1],
        };

        // Initialize the free list with the maximum number of power-of-two
        // blocks that will fit.
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

        // Continuously split blocks until the smallest possible order that will
        // fit.
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
            blk_idx < self.total_blks - self.alloc_area_blk_idx,
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
            let buddy_info = unsafe { &mut *self.info.add(buddy_blk_idx) };

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
        self.alloc_area_addr + (blk_idx * BLK_SIZE)
    }

    fn addr_to_blk_idx(&self, addr: usize) -> usize {
        (addr - self.alloc_area_addr) / BLK_SIZE
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

    #[allow(dead_code)]
    pub fn dbgdump(&self) {
        dbgprintln!(
            "total_blks={} alloc_area_addr={:#x} alloc_area_blk_idx={}",
            self.total_blks,
            self.alloc_area_addr,
            self.alloc_area_blk_idx
        );

        dbgprintln!("blk info:");
        let mut blk_idx = 0;
        while blk_idx < self.total_blks - self.alloc_area_blk_idx {
            let info = unsafe { &mut *self.info.add(blk_idx) };
            let addr = self.blk_idx_to_addr(blk_idx);

            // Step through the blocks by their coalesced size.
            let blk_cnt = 1usize << info.order.min(MAX_ORDER as u8);
            dbgprintln!(
                "  {:#010x}-{:#010x}  order {:<2} ({:>7} bytes)  {}",
                addr,
                addr + blk_cnt * BLK_SIZE,
                info.order,
                blk_cnt * BLK_SIZE,
                if info.free { "free" } else { "used" },
            );
            blk_idx += blk_cnt;
        }

        dbgprintln!("free lists:");
        for order in 0..=(MAX_ORDER as u8) {
            dbgprint!(
                "  order {:<2} ({:>7} bytes):",
                order,
                (1usize << order) * BLK_SIZE
            );
            let mut cnt = 0usize;
            let mut head = self.free_lists[order as usize];
            while let Some(blk_ptr) = head {
                head = unsafe { blk_ptr.as_ref().next };
                cnt += 1;
            }
            if cnt == 0 {
                dbgprintln!(" (empty)");
            } else {
                dbgprintln!(" {} block(s)", cnt);
            }
        }
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

        fn base_addr(&self) -> usize {
            self.ptr as usize
        }
    }

    impl Drop for TestRegion {
        fn drop(&mut self) {
            unsafe { std::alloc::dealloc(self.ptr, self.layout) };
        }
    }

    #[test]
    fn alloc_and_free_order() {
        let region = TestRegion::new(8 * BLK_SIZE);
        let mut allocator = BuddyAllocator::new(region.base_addr(), region.layout.size());

        // Block 0 is used for info.
        //
        //          | 1234567
        // start    | -------
        // alloc p1 | ****---
        // alloc p2 | ****-**
        // alloc p3 | *******
        // free p1  | ----***
        // alloc p4 | *---***
        // alloc p5 | *---*** - should fail
        // free p4  | ----***
        // free p3  | -----**
        // free p2  | -------

        let p1 = allocator.alloc(2).expect("failed to alloc p1");
        assert_eq!(p1, region.base_addr() + (1 * BLK_SIZE));

        let p2 = allocator.alloc(1).expect("failed to alloc p2");
        assert_eq!(p2, region.base_addr() + (6 * BLK_SIZE));

        let p3 = allocator.alloc(0).expect("failed to alloc p3");
        assert_eq!(p3, region.base_addr() + (5 * BLK_SIZE));

        allocator.free(p1, 2);

        let p4 = allocator.alloc(0).expect("failed to alloc p4");
        assert_eq!(p4, region.base_addr() + (1 * BLK_SIZE));

        let p5 = allocator.alloc(2);
        assert_eq!(p5, None);

        allocator.free(p4, 0);
        allocator.free(p3, 0);
        allocator.free(p2, 1);

        let count_list = |mut head: Option<NonNull<FreeBlock>>| {
            let mut cnt = 0;
            while let Some(blk_ptr) = head {
                head = unsafe { blk_ptr.as_ref().next };
                cnt += 1;
            }
            cnt
        };

        assert_eq!(1, count_list(allocator.free_lists[2]));
        assert_eq!(1, count_list(allocator.free_lists[1]));
        assert_eq!(1, count_list(allocator.free_lists[0]));
    }

    #[test]
    fn alloc_and_free_pages() {
        let region = TestRegion::new(8 * BLK_SIZE);
        let mut allocator = BuddyAllocator::new(region.base_addr(), region.layout.size());

        // Block 0 is used for info.
        //
        //          | 1234567
        // start    | -------
        // alloc p1 | ****--- - should round-up
        // alloc p2 | ****-**
        // alloc p3 | *******
        // free p1  | ----*** - should round-up
        // free p2  | ----*--
        // free p3  | ----_--

        let p1 = allocator.alloc_pages(3).expect("failed to alloc p1");
        assert_eq!(p1, region.base_addr() + (1 * BLK_SIZE));

        let p2 = allocator.alloc_pages(2).expect("failed to alloc p2");
        assert_eq!(p2, region.base_addr() + (6 * BLK_SIZE));

        let p3 = allocator.alloc_pages(1).expect("failed to alloc p3");
        assert_eq!(p3, region.base_addr() + (5 * BLK_SIZE));

        allocator.free_pages(p1, 3);
        allocator.free_pages(p2, 2);
        allocator.free_pages(p3, 1);
    }

    #[test]
    #[should_panic(expected = "block-aligned")]
    fn misaligned_base_panics() {
        let region = TestRegion::new(8 * BLK_SIZE);
        BuddyAllocator::new(region.base_addr() + 10, region.layout.size());
    }

    #[test]
    #[should_panic(expected = "too small")]
    fn region_too_small_panics() {
        let region = TestRegion::new(BLK_SIZE - 10);
        BuddyAllocator::new(region.base_addr(), region.layout.size());
    }

    #[test]
    #[should_panic(expected = "double free")]
    fn double_free_panics() {
        let region = TestRegion::new(8 * BLK_SIZE);
        let mut allocator = BuddyAllocator::new(region.base_addr(), region.layout.size());

        let p1 = allocator.alloc(2).expect("failed to alloc p1");
        assert_eq!(p1, region.base_addr() + (1 * BLK_SIZE));

        allocator.free(p1, 2);
        allocator.free(p1, 2);
    }
}
