#ifndef CPU_IDT_H
#define CPU_IDT_H

#include <stdint.h>

#define IDT_ENTRY_LIMIT 256

struct idt_entry_struct {
  uint16_t base_low;  // Lower 16 bits of address to jump to
  uint16_t selector;  // Kernel segment selector
  uint8_t always0;
  uint8_t flags;
  uint16_t base_high; // Upper 16 bits of address to jump to
} __attribute__ ((packed));

typedef struct idt_entry_struct idt_entry;

struct idt_pointer_struct {
  uint16_t limit;
  uint32_t base;
} __attribute__ ((packed));

typedef struct idt_pointer_struct idt_pointer;

typedef struct registers_struct {
  uint32_t ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32_t int_no, err_code;
  uint32_t eip, cs, eflags, useresp, ss;
} registers;

void idt_init();

void isr_handler(registers r);

void idt_flush(uint32_t base);

void _exception0();
void _exception1();
void _exception2();
void _exception3();
void _exception4();
void _exception5();
void _exception6();
void _exception7();
void _exception8();
void _exception9();
void _exception10();

#endif
