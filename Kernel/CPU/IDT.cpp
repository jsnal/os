#include <Kernel/CPU/IDT.h>
#include <Kernel/CPU/IRQHandler.h>
#include <Kernel/CPU/PIC.h>
#include <Kernel/Logger.h>
#include <Kernel/panic.h>
#include <Universal/Number.h>
#include <Universal/Stdlib.h>

#define IDT_ENTRY_LIMIT 256
#define EXCEPTION_HANDLER_COUNT 32

static ExceptionHandler s_exception_handlers[EXCEPTION_HANDLER_COUNT];

[[gnu::aligned(0x10)]] static IDTEntry s_idt_entries[IDT_ENTRY_LIMIT];

static IDTPointer s_idt_pointer;

static void divide_by_zero_exception_handler(const InterruptFrame&)
{
    panic("Divide by zero detected!\n");
}

static void idt_set_entry(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
    s_idt_entries[num].base_low = base & 0xFFFF;
    s_idt_entries[num].base_high = (base >> 16) & 0xFFFF;
    s_idt_entries[num].selector = selector;
    s_idt_entries[num].zero = 0;
    s_idt_entries[num].flags = flags;
}

extern "C" void isr_handler(InterruptFrame* frame)
{
    if (frame == nullptr || frame->interrupt_number > EXCEPTION_HANDLER_COUNT + IRQHandler::handler_count - 1) {
        if (frame == nullptr) {
            panic("Interrupt ? is not handled! Error ?!\n");
        }
        panic("Interrupt %d is not handled! Error %d!\n", frame->interrupt_number, frame->error_number);
    }

    if (number_between_inclusive(frame->interrupt_number, 0, 31)) {
        if (s_exception_handlers[frame->interrupt_number] != nullptr) {
            s_exception_handlers[frame->interrupt_number](*frame);
        }

        dbgprintf("IDT", "Interrupt fired: %d\n", frame->interrupt_number);
        IDT::dump_interrupt_frame(*frame);
        panic("");
    } else {
        IRQHandler::handle_all_irqs(*frame);
    }
}

namespace IDT {

Result register_exception_handler(u32 exception_number, ExceptionHandler handler)
{
    if (exception_number < EXCEPTION_HANDLER_COUNT) {
        s_exception_handlers[exception_number] = handler;
        return Result::OK;
    }

    return Result::Failure;
}

void dump_interrupt_frame(const InterruptFrame& frame)
{
    dbgprintf("IDT", "EAX=%x EBX=%x ECX=%x EDX=%x\n", frame.eax, frame.ebx, frame.ecx, frame.edx);
    dbgprintf("IDT", "ESI=%x EDI=%x EBP=%x ESP=%x\n", frame.esi, frame.edi, frame.ebp, frame.esp);
    dbgprintf("IDT", "DS=%x CS=%x SS=%x\n", frame.ds, frame.cs, frame.ss);
    dbgprintf("IDT", "EFLAGS=%x\n", frame.eflags);
}

void init()
{
    s_idt_pointer.limit = (sizeof(s_idt_pointer) * IDT_ENTRY_LIMIT) - 1;
    s_idt_pointer.base = (uintptr_t)&s_idt_entries;

    memset(&s_idt_entries, 0, sizeof(IDTEntry) * IDT_ENTRY_LIMIT);

    // CPU interrupts for exceptions
    idt_set_entry(0, (uintptr_t)isr_0, 0x08, 0x8E);
    idt_set_entry(1, (uintptr_t)isr_1, 0x08, 0x8E);
    idt_set_entry(2, (uintptr_t)isr_2, 0x08, 0x8E);
    idt_set_entry(3, (uintptr_t)isr_3, 0x08, 0x8E);
    idt_set_entry(4, (uintptr_t)isr_4, 0x08, 0x8E);
    idt_set_entry(5, (uintptr_t)isr_5, 0x08, 0x8E);
    idt_set_entry(6, (uintptr_t)isr_6, 0x08, 0x8E);
    idt_set_entry(7, (uintptr_t)isr_7, 0x08, 0x8E);
    idt_set_entry(8, (uintptr_t)isr_8, 0x08, 0x8E);
    idt_set_entry(9, (uintptr_t)isr_9, 0x08, 0x8E);
    idt_set_entry(10, (uintptr_t)isr_10, 0x08, 0x8E);
    idt_set_entry(11, (uintptr_t)isr_11, 0x08, 0x8E);
    idt_set_entry(12, (uintptr_t)isr_12, 0x08, 0x8E);
    idt_set_entry(13, (uintptr_t)isr_13, 0x08, 0x8E);
    idt_set_entry(14, (uintptr_t)isr_14, 0x08, 0x8E);
    idt_set_entry(15, (uintptr_t)isr_15, 0x08, 0x8E);
    idt_set_entry(16, (uintptr_t)isr_16, 0x08, 0x8E);
    idt_set_entry(17, (uintptr_t)isr_17, 0x08, 0x8E);
    idt_set_entry(18, (uintptr_t)isr_18, 0x08, 0x8E);
    idt_set_entry(19, (uintptr_t)isr_19, 0x08, 0x8E);
    idt_set_entry(20, (uintptr_t)isr_20, 0x08, 0x8E);
    idt_set_entry(21, (uintptr_t)isr_21, 0x08, 0x8E);
    idt_set_entry(22, (uintptr_t)isr_22, 0x08, 0x8E);
    idt_set_entry(23, (uintptr_t)isr_23, 0x08, 0x8E);
    idt_set_entry(24, (uintptr_t)isr_24, 0x08, 0x8E);
    idt_set_entry(25, (uintptr_t)isr_25, 0x08, 0x8E);
    idt_set_entry(26, (uintptr_t)isr_26, 0x08, 0x8E);
    idt_set_entry(27, (uintptr_t)isr_27, 0x08, 0x8E);
    idt_set_entry(28, (uintptr_t)isr_28, 0x08, 0x8E);
    idt_set_entry(29, (uintptr_t)isr_29, 0x08, 0x8E);
    idt_set_entry(30, (uintptr_t)isr_30, 0x08, 0x8E);
    idt_set_entry(31, (uintptr_t)isr_31, 0x08, 0x8E);

    // Hardware devivce interrupts
    idt_set_entry(32, (uintptr_t)isr_32, 0x08, 0x8E);
    idt_set_entry(33, (uintptr_t)isr_33, 0x08, 0x8E);
    idt_set_entry(34, (uintptr_t)isr_34, 0x08, 0x8E);
    idt_set_entry(35, (uintptr_t)isr_35, 0x08, 0x8E);
    idt_set_entry(36, (uintptr_t)isr_36, 0x08, 0x8E);
    idt_set_entry(37, (uintptr_t)isr_37, 0x08, 0x8E);
    idt_set_entry(38, (uintptr_t)isr_38, 0x08, 0x8E);
    idt_set_entry(39, (uintptr_t)isr_39, 0x08, 0x8E);
    idt_set_entry(40, (uintptr_t)isr_40, 0x08, 0x8E);
    idt_set_entry(41, (uintptr_t)isr_41, 0x08, 0x8E);
    idt_set_entry(42, (uintptr_t)isr_42, 0x08, 0x8E);
    idt_set_entry(43, (uintptr_t)isr_43, 0x08, 0x8E);
    idt_set_entry(44, (uintptr_t)isr_44, 0x08, 0x8E);
    idt_set_entry(45, (uintptr_t)isr_45, 0x08, 0x8E);
    idt_set_entry(46, (uintptr_t)isr_46, 0x08, 0x8E);
    idt_set_entry(47, (uintptr_t)isr_47, 0x08, 0x8E);

    idt_set_entry(128, (uintptr_t)isr_syscall, 0x08, 0xEF);

    idt_load((uintptr_t)&s_idt_pointer);

    dbgprintf("IDT", "Initialized IDT: 0x%x\n", &s_idt_pointer);

    if (register_exception_handler(EXCEPTION_DIVIDE_BY_ZERO, divide_by_zero_exception_handler).is_error()) {
        panic("Unable to register 'divide by zero' exception handler\n");
    }
}
}
