use core::{arch::asm, fmt, mem, ptr::addr_of};

use crate::dbgprintln;

struct AccessFlags;

impl AccessFlags {
    const PRESENT: u8 = 1 << 7;
    const KERNEL: u8 = 0 << 5;
    const USER: u8 = 3 << 5;
    const DATA_OR_CODE: u8 = 1 << 4;
    const EXECUTABLE: u8 = 1 << 3;
    const READABLE_OR_WRITEABLE: u8 = 1 << 1;
}

#[repr(C, packed)]
struct SegmentDescriptor {
    limit_low: u16,
    base_low: u16,
    base_middle: u8,
    access: u8,
    flags_and_limit_high: u8,
    base_high: u8,
}

impl SegmentDescriptor {
    const NULL: Self = Self {
        limit_low: 0,
        base_low: 0,
        base_middle: 0,
        access: 0,
        flags_and_limit_high: 0,
        base_high: 0,
    };

    /// Use the following defaults:
    ///     Base: 0
    ///     Limit: 0xFFFFF - Span the entire 4GB address space
    ///     Flags: 0xC0 - Sets the limit granularity to 4KB blocks and defines
    ///            a 32-bit protected mode segment
    const fn new(access_flags: u8) -> Self {
        Self {
            limit_low: 0xFFFF,
            base_low: 0,
            base_middle: 0,
            access: access_flags,
            flags_and_limit_high: 0xCF,
            base_high: 0,
        }
    }

    pub fn raw(&self) -> u64 {
        (self.limit_low as u64)
            | (self.base_low as u64) << 16
            | (self.base_middle as u64) << 32
            | (self.access as u64) << 40
            | (self.flags_and_limit_high as u64) << 48
            | (self.base_high as u64) << 56
    }
}

impl fmt::Debug for SegmentDescriptor {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "SegmentDescriptor({:#018x})", self.raw())
    }
}

#[repr(C, packed)]
struct GdtDescriptor {
    size: u16,
    offset: u32,
}

impl GdtDescriptor {
    const fn new(size: u16, offset: u32) -> Self {
        Self { size, offset }
    }

    pub fn raw(&self) -> u64 {
        (self.size as u64) | (self.offset as u64) << 16
    }
}

pub struct SegmentDescriptorIndex;

impl SegmentDescriptorIndex {
    pub const KERNEL_CODE: u16 = 1;
    pub const KERNEL_DATA: u16 = 2;
}

pub struct SegmentRingLevel;

impl SegmentRingLevel {
    pub const KERNEL: u16 = 0;
}

#[repr(C, packed)]
#[derive(Clone, Copy)]
pub struct SegmentSelector(u16);

impl SegmentSelector {
    pub const NULL: Self = Self(0);

    pub const fn new(index: u16, ring: u16) -> Self {
        Self(index << 3 | (ring as u16))
    }

    pub fn raw(&self) -> u16 {
        self.0
    }
}

impl fmt::Debug for SegmentSelector {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "SegmentSelector({:#x})", self.raw())
    }
}

const SEGMENT_DESCRIPTOR_COUNT: usize = 5;
static GDT: [SegmentDescriptor; SEGMENT_DESCRIPTOR_COUNT] = [
    SegmentDescriptor::NULL,
    SegmentDescriptor::new(
        AccessFlags::PRESENT
            | AccessFlags::KERNEL
            | AccessFlags::DATA_OR_CODE
            | AccessFlags::EXECUTABLE
            | AccessFlags::READABLE_OR_WRITEABLE,
    ),
    SegmentDescriptor::new(
        AccessFlags::PRESENT
            | AccessFlags::KERNEL
            | AccessFlags::DATA_OR_CODE
            | AccessFlags::READABLE_OR_WRITEABLE,
    ),
    SegmentDescriptor::new(
        AccessFlags::PRESENT
            | AccessFlags::USER
            | AccessFlags::DATA_OR_CODE
            | AccessFlags::EXECUTABLE
            | AccessFlags::READABLE_OR_WRITEABLE,
    ),
    SegmentDescriptor::new(
        AccessFlags::PRESENT
            | AccessFlags::USER
            | AccessFlags::DATA_OR_CODE
            | AccessFlags::READABLE_OR_WRITEABLE,
    ),
];

pub fn init() {
    let descriptor = GdtDescriptor::new(
        mem::size_of::<[SegmentDescriptor; SEGMENT_DESCRIPTOR_COUNT]>() as u16,
        addr_of!(GDT).addr() as u32,
    );

    // Load the GDTR with a pointer to the static table
    unsafe {
        asm!("lgdt [{}]", in(reg) &descriptor, options(nomem, nostack));
    }

    // All segment selector registers should point to the Kernel's data segment except for CS which
    // must be loaded separately
    unsafe {
        asm!(
            "mov ds, ax",
            "mov es, ax",
            "mov fs, ax",
            "mov gs, ax",
            "mov ss, ax",
            in("ax") SegmentSelector::new(SegmentDescriptorIndex::KERNEL_DATA, SegmentRingLevel::KERNEL).raw()
        );
    }

    // Set the CS selector to the Kernel's code segment
    unsafe {
        core::arch::asm!(
            "push {selector:e}", // Push the new CS selector onto the stack
            "lea eax, [2f]",     // Load the address of label '2' into eax
            "push eax",          // Push that offset onto the stack
            "retf",              // Far return to set EIP and CS at the same time
            "2:",
            selector = in(reg) SegmentSelector::new(SegmentDescriptorIndex::KERNEL_CODE, SegmentRingLevel::KERNEL).raw(),
            out("eax") _,
        );
    }

    dbgprintln!("initialized GDT: gdtr={:#016x}", descriptor.raw());
}
