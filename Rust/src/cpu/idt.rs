use core::{
    arch::{asm, global_asm},
    fmt, mem,
    ptr::addr_of,
};

use crate::{
    cpu::{
        gdt::{SegmentDescriptorIndex, SegmentRingLevel, SegmentSelector},
        pic,
    },
    dbgprintln,
};

global_asm!(include_str!("isrs.s"));

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
    const fn new() -> Self {
        Self {
            offset_low: 0,
            selector: SegmentSelector::NULL,
            reserved: 0,
            flags: 0,
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

#[repr(C, packed)]
#[derive(Debug)]
pub struct InterruptStackFrame {
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

pub type InterruptHandler = fn(&mut InterruptStackFrame);

macro_rules! define_exception_handler {
    ($($name:ident => $msg:expr),*) => {
        $(
            fn $name(stack_frame: &mut InterruptStackFrame) {
                let int_no = stack_frame.int_no;
                panic!("unrecoverable interrupt: {} ({})\n{:#x?}", $msg, int_no, stack_frame);
            }
        )*
    };
}

const GATE_DESCRIPTOR_COUNT: usize = 256;

unsafe extern "C" {
    // The list of all the ISR assembly routines. Defined in 'isrs.asm'
    static isrs: [*const u8; GATE_DESCRIPTOR_COUNT];
}

static mut IDT: [GateDescriptor; GATE_DESCRIPTOR_COUNT] =
    [GateDescriptor::new(); GATE_DESCRIPTOR_COUNT];

static mut INTERRUPT_HANDLERS: [Option<InterruptHandler>; GATE_DESCRIPTOR_COUNT] =
    [None; GATE_DESCRIPTOR_COUNT];

define_exception_handler!(
    division_error => "division error",
    debug => "debug",
    non_maskable_interrupt => "non-maskable interrupt",
    breakpoint => "breakpoint",
    overflow => "overflow",
    bound_range_exceeded => "bound range exceeded",
    invalid_opcode => "invalid opcode",
    device_not_available => "device not available",
    double_fault => "double fault",
    invalid_tss => "invalid tss",
    segment_not_present => "segment not present",
    stack_segment_fault => "stack-segment fault",
    general_protection_fault => "general protection fault",
    page_fault => "page fault",
    x87_floating_point_exception => "x87 floating-point exception",
    alignment_check => "alignment check",
    machine_check => "machine check",
    simd_floating_point_exception => "simd floating-point exception",
    virtualization_exception => "virtualization exception",
    control_protection_exception => "control protection exception"
);

/// Entry point from assembly after an interrupt fires. All this function does is call out to the
/// real interrupt handler which will take any necessary actions.
#[unsafe(no_mangle)]
fn interrupt_handler_dispatcher(stack_frame: &mut InterruptStackFrame) {
    if let Some(handler) = unsafe { INTERRUPT_HANDLERS[stack_frame.int_no as usize] } {
        handler(stack_frame);

        // Send the EOI for IRQs based on where the PIC was relocated
        if (stack_frame.int_no as u8) >= pic::PIC1_OFFSET {
            pic::eoi((stack_frame.int_no as u8) - pic::PIC1_OFFSET);
        }
    } else {
        let int_no = stack_frame.int_no;
        dbgprintln!("unhandled interrupt: {}", int_no);
    }
}

/// # Panics
/// * If an interrupt already exists for the given interrupt index
fn set_interrupt_handler(index: u8, handler: InterruptHandler) {
    if let Some(_) = unsafe { INTERRUPT_HANDLERS[index as usize] } {
        panic!("interrupt handler already registered, index={}", index);
    }
    unsafe {
        INTERRUPT_HANDLERS[index as usize] = Some(handler);
    }
}

/// # Panics
/// * If the interrupt index is too large
/// * If an interrupt already exists for the given interrupt index
pub fn set_exception_handler(index: u8, handler: InterruptHandler) {
    if index >= 32 {
        panic!("interrupt index is too large, index={}", index);
    }
    set_interrupt_handler(index, handler);
}

/// # Panics
/// * If an interrupt already exists for the given interrupt index
pub fn set_irq_handler(index: u8, handler: InterruptHandler) {
    set_interrupt_handler(index + pic::PIC1_OFFSET, handler);
}

pub fn init() {
    unsafe {
        for (index, &handler) in isrs.iter().enumerate() {
            if handler.addr() == 0 {
                continue;
            }
            IDT[index].set_isr(handler.addr());
        }
    }

    let descriptor = IdtDescriptor::new(
        mem::size_of::<[GateDescriptor; GATE_DESCRIPTOR_COUNT]>() as u16,
        addr_of!(IDT).addr() as u32,
    );

    // Load the LDTR with a pointer to the static table
    unsafe {
        asm!("lidt [{}]", in(reg) &descriptor, options(nomem, nostack));
    }

    // Load all of the CPU exceptions into their interrupt handler
    set_exception_handler(0, division_error);
    set_exception_handler(1, debug);
    set_exception_handler(2, non_maskable_interrupt);
    set_exception_handler(3, breakpoint);
    set_exception_handler(4, overflow);
    set_exception_handler(5, bound_range_exceeded);
    set_exception_handler(6, invalid_opcode);
    set_exception_handler(7, device_not_available);
    set_exception_handler(8, double_fault);
    set_exception_handler(10, invalid_tss);
    set_exception_handler(11, segment_not_present);
    set_exception_handler(12, stack_segment_fault);
    set_exception_handler(13, general_protection_fault);
    set_exception_handler(14, page_fault);
    set_exception_handler(16, x87_floating_point_exception);
    set_exception_handler(17, alignment_check);
    set_exception_handler(18, machine_check);
    set_exception_handler(19, simd_floating_point_exception);
    set_exception_handler(20, virtualization_exception);
    set_exception_handler(21, control_protection_exception);

    dbgprintln!("loaded IDT: idtr={:#016x}", descriptor.raw());
}
