//! Test only utility helpers.

use core::alloc::Layout;

use crate::mm::PAGE_SIZE;

pub struct TestRegion {
    ptr: *mut u8,
    layout: Layout,
}

impl TestRegion {
    pub fn new(size: usize) -> Self {
        let layout = Layout::from_size_align(size, PAGE_SIZE).expect("invalid layout");
        let ptr = unsafe { std::alloc::alloc_zeroed(layout) };
        assert!(!ptr.is_null(), "allocation failed");
        Self { ptr, layout }
    }

    pub fn base_addr(&self) -> usize {
        self.ptr as usize
    }

    pub fn size(&self) -> usize {
        self.layout.size()
    }
}

impl Drop for TestRegion {
    fn drop(&mut self) {
        unsafe { std::alloc::dealloc(self.ptr, self.layout) };
    }
}
