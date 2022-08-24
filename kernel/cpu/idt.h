#ifndef CPU_IDT_H
#define CPU_IDT_H

#include <stdint.h>

#define IDT_ENTRY_LIMIT 256

typedef struct idt_entry_struct {
  uint16_t base_low;  // Lower 16 bits of address to jump to
  uint16_t selector;  // Kernel segment selector
  uint8_t zero;
  uint8_t flags;
  uint16_t base_high; // Upper 16 bits of address to jump to
} __attribute__ ((packed)) idt_entry_t;

typedef struct idtr_struct {
  uint16_t limit;
  uint32_t base;
} __attribute__ ((packed)) idtr_t;

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
void _exception11();
void _exception12();
void _exception13();
void _exception14();
void _exception15();
void _exception16();
void _exception17();
void _exception18();
void _exception19();
void _exception20();
void _exception21();
void _exception22();
void _exception23();
void _exception24();
void _exception25();
void _exception26();
void _exception27();
void _exception28();
void _exception29();
void _exception30();
void _exception31();

#endif
