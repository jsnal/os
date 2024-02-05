#ifndef CPU_IDT_H
#define CPU_IDT_H

#include <stdint.h>

#define IDT_ENTRY_LIMIT 256
#define IDT_ENTRY_COUNT 48

#define ISR_PAGE_FAULT 14
#define ISR_PIT 32

typedef void (*isr_handler_t)(void);

typedef struct idt_entry_struct {
    uint16_t base_low; // Lower 16 bits of address to jump to
    uint16_t selector; // Kernel segment selector
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high; // Upper 16 bits of address to jump to
} __attribute__((packed)) idt_entry_t;

typedef struct idtr_struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

typedef struct isr_frame_struct {
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t int_no;
    uint32_t err_no;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t usr_esp;
    uint32_t ss;
} __attribute__((packed)) isr_frame_t;

static inline void sti()
{
    asm volatile("sti");
}

static inline void cli()
{
    asm volatile("cli");
}

void idt_load(uint32_t base);

void isr_handler(isr_frame_t*);

void isr_register_handler(uint32_t int_no, isr_handler_t);

void idt_init();

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

#endif
