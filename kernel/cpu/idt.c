#include <api/printf.h>
#include <api/string.h>
#include <cpu/idt.h>
#include <cpu/panic.h>
#include <cpu/pic.h>
#include <devices/vga.h>
#include <logger/logger.h>

__attribute__((aligned(0x10))) static idt_entry_t idt_entries[IDT_ENTRY_LIMIT];

static idtr_t idtr;

static void idt_set_entry(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
    idt_entries[num].base_low = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;
    idt_entries[num].selector = selector;
    idt_entries[num].zero = 0;
    idt_entries[num].flags = flags;
}

void isr_handler(isr_frame_t* isr_frame)
{
    if (isr_frame->int_no == 33) {
        dbgln("Keyboard!\n");
        return;
    }

    dbgln("Interrupt fired: %d:%x\n", isr_frame->int_no, isr_frame->int_no);
    panic("Unhandled interrupt");
}

void idt_init()
{
    idtr.limit = (sizeof(idtr_t) * IDT_ENTRY_LIMIT) - 1;
    idtr.base = (uintptr_t)&idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entry_t) * IDT_ENTRY_LIMIT);

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

    idt_load((uintptr_t)&idtr);
}
