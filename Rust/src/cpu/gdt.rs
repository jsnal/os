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
struct Entry {
    limit_low: u16,
    base_low: u16,
    base_middle: u8,
    access: u8,
    flags_and_limit_high: u8,
    base_high: u8,
}

impl Entry {
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

impl fmt::Debug for Entry {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Entry({:#018x})", self.raw())
    }
}

#[repr(C, packed)]
struct TablePointer {
    size: u16,
    offset: u32,
}

impl TablePointer {
    const fn new(size: u16, offset: u32) -> Self {
        Self { size, offset }
    }
}

struct EntryIndex;

impl EntryIndex {
    const KERNEL_CODE: u16 = 1;
    const KERNEL_DATA: u16 = 2;
}

struct RingLevel;

impl RingLevel {
    const KERNEL: u16 = 0;
    const USER: u16 = 3;
}

pub struct SegmentSelector(u16);

impl SegmentSelector {
    pub const fn new(index: u16, ring: u16) -> Self {
        Self(index << 3 | (ring as u16))
    }

    pub fn raw(&self) -> u16 {
        self.0
    }
}

impl fmt::Debug for SegmentSelector {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "SegmentSelector({:#018x})", self.raw())
    }
}

const ENTRY_COUNT: usize = 5;
static GDT: [Entry; ENTRY_COUNT] = [
    Entry::NULL,
    Entry::new(
        AccessFlags::PRESENT
            | AccessFlags::KERNEL
            | AccessFlags::DATA_OR_CODE
            | AccessFlags::EXECUTABLE
            | AccessFlags::READABLE_OR_WRITEABLE,
    ),
    Entry::new(
        AccessFlags::PRESENT
            | AccessFlags::KERNEL
            | AccessFlags::DATA_OR_CODE
            | AccessFlags::READABLE_OR_WRITEABLE,
    ),
    Entry::new(
        AccessFlags::PRESENT
            | AccessFlags::USER
            | AccessFlags::DATA_OR_CODE
            | AccessFlags::EXECUTABLE
            | AccessFlags::READABLE_OR_WRITEABLE,
    ),
    Entry::new(
        AccessFlags::PRESENT
            | AccessFlags::USER
            | AccessFlags::DATA_OR_CODE
            | AccessFlags::READABLE_OR_WRITEABLE,
    ),
];

pub fn init() {
    let table_pointer = TablePointer::new(
        mem::size_of::<[Entry; ENTRY_COUNT]>() as u16,
        addr_of!(GDT).addr() as u32,
    );

    // Load the GDTR with a pointer to the static table
    unsafe {
        asm!("lgdt [{}]", in(reg) &table_pointer, options(nomem, nostack));
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
            in("ax") SegmentSelector::new(EntryIndex::KERNEL_DATA, RingLevel::KERNEL).raw()
        );
    }

    // TODO: Load the CS register? Not sure how yet.
    dbgprintln!("Loaded GDT: {:p}", &GDT);
}
