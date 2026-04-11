use core::{
    arch::{asm, global_asm},
    fmt, mem,
    ptr::addr_of,
};

use crate::{
    cpu::gdt::{SegmentDescriptorIndex, SegmentRingLevel, SegmentSelector},
    dbgprintln,
};

global_asm!(include_str!("handlers.asm"));

struct GateDescriptorFlags;

impl GateDescriptorFlags {
    const PRESENT: u8 = 1 << 7;
    const KERNEL: u8 = 0 << 5;
    const INTERRUPT: u8 = 0xE;
}

#[repr(C, packed)]
#[derive(Clone, Copy)]
struct GateDescriptor {
    offset_low: u16,
    selector: SegmentSelector,
    reserved: u8,
    flags: u8,
    offset_high: u16,
}

impl GateDescriptor {
    const fn new(flags: u8) -> Self {
        Self {
            offset_low: 0,
            selector: SegmentSelector::NULL,
            reserved: 0,
            flags,
            offset_high: 0,
        }
    }

    pub fn set_isr(&mut self, offset: usize) {
        self.offset_low = ((offset as u32) & 0xFFFF) as u16;
        self.selector = SegmentSelector::new(
            SegmentDescriptorIndex::KERNEL_CODE,
            SegmentRingLevel::KERNEL,
        );
        self.flags = GateDescriptorFlags::PRESENT
            | GateDescriptorFlags::KERNEL
            | GateDescriptorFlags::INTERRUPT;
        self.offset_high = (((offset as u32) >> 16) & 0xFFFF) as u16;
    }

    pub fn raw(&self) -> u64 {
        (self.offset_low as u64)
            | (self.selector.raw() as u64) << 16
            | (self.reserved as u64) << 32
            | (self.flags as u64) << 40
            | (self.offset_high as u64) << 48
    }
}

impl fmt::Debug for GateDescriptor {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "GateDescriptor({:#x})", self.raw())
    }
}

#[repr(C, packed)]
struct IdtDescriptor {
    size: u16,
    offset: u32,
}

impl IdtDescriptor {
    const fn new(size: u16, offset: u32) -> Self {
        Self { size, offset }
    }

    pub fn raw(&self) -> u64 {
        (self.size as u64) | (self.offset as u64) << 16
    }
}

const GATE_DESCRIPTOR_COUNT: usize = 256;

unsafe extern "C" {
    static isrs: [*const u8; GATE_DESCRIPTOR_COUNT];
}

static mut IDT: [GateDescriptor; GATE_DESCRIPTOR_COUNT] =
    [GateDescriptor::new(0x8E); GATE_DESCRIPTOR_COUNT];

#[repr(C, packed)]
#[derive(Debug)]
struct StackFrame {
    // Segment registers push manually
    gs: u32,
    fs: u32,
    es: u32,
    ds: u32,

    // General purpose registered pushed with 'pusha'
    edi: u32,
    esi: u32,
    ebp: u32,
    esp: u32,
    ebx: u32,
    edx: u32,
    ecx: u32,
    eax: u32,

    // Pushed by the 'isr_common' assembly routine
    int_no: u32,
    err_code: u32,

    // Pushed by the CPU automatically
    eip: u32,
    cs: u32,
    eflags: u32,
    user_esp: u32,
    ss: u32,
}

#[unsafe(no_mangle)]
fn isr_handler(stack_frame: &StackFrame) {
    //dbgprintln!("Interrupt fired: {}", stack_frame.int_no);
    dbgprintln!("{:#x?}", stack_frame);
    //panic!("Oh no");
}

pub fn init() {
    unsafe {
        for (index, &handler) in isrs.iter().enumerate() {
            IDT[index].set_isr(handler.addr());
        }
    }

    let descriptor = IdtDescriptor::new(
        mem::size_of::<[GateDescriptor; GATE_DESCRIPTOR_COUNT]>() as u16,
        addr_of!(IDT).addr() as u32,
    );

    unsafe {
        asm!("lidt [{}]", in(reg) &descriptor, options(nomem, nostack));
    }

    dbgprintln!("Loaded IDT: {:#x}", descriptor.raw());

    unsafe { asm!("int 5") }
}
