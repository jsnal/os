#pragma once

#include <Universal/Types.h>

#define ISR_PAGE_FAULT 14
#define ISR_PIT 32
#define ISR_KEYBOARD 33
#define ISR_DISK 46

struct [[gnu::packed]] IDTEntry {
    u16 base_low;
    u16 selector;
    u8 zero;
    u8 flags;
    u16 base_high;
};

struct [[gnu::packed]] IDTPointer {
    u16 limit;
    u32 base;
};

struct InterruptFrame {
    u32 ds;
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 esp;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    u32 interrupt_number;
    u32 error_number;
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 user_esp;
    u32 ss;
};

typedef void (*InterruptHandler)();

static inline void sti()
{
    asm volatile("sti");
}

static inline void cli()
{
    asm volatile("cli");
}

namespace IDT {

void register_interrupt_handler(u32 interrupt_number, InterruptHandler);

void dump_interrupt_frame(const InterruptFrame&);

void init();

}

extern "C" {

void idt_load(u32 base);

void isr_handler(InterruptFrame*);

void isr_0();
void isr_1();
void isr_2();
void isr_3();
void isr_4();
void isr_5();
void isr_6();
void isr_7();
void isr_8();
void isr_9();
void isr_10();
void isr_11();
void isr_12();
void isr_13();
void isr_14();
void isr_15();
void isr_16();
void isr_17();
void isr_18();
void isr_19();
void isr_20();
void isr_21();
void isr_22();
void isr_23();
void isr_24();
void isr_25();
void isr_26();
void isr_27();
void isr_28();
void isr_29();
void isr_30();
void isr_31();
void isr_32();
void isr_33();
void isr_34();
void isr_35();
void isr_36();
void isr_37();
void isr_38();
void isr_39();
void isr_40();
void isr_41();
void isr_42();
void isr_43();
void isr_44();
void isr_45();
void isr_46();
void isr_47();
}
