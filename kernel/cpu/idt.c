#include <api/string.h>
#include <cpu/idt.h>
#include <devices/vga.h>

static idt_entry idt_entries[IDT_ENTRY_LIMIT];
static idt_pointer idt;

static void idt_set_entry(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
  idt_entries[num].base_low = base & 0xFFFF;
  idt_entries[num].base_high = (base >> 16) & 0xFFFF;
  idt_entries[num].selector = selector;
  idt_entries[num].always0 = 0;
  idt_entries[num].flags = flags;
}

void isr_handler(registers r)
{
  vga_write("Interupt found\n");
}

void idt_init()
{
  vga_write("Hello from idt init!\n");

  idt.limit = (sizeof(idt_pointer) * IDT_ENTRY_LIMIT) - 1;
  idt.base = (uint32_t) &idt_entries;

  memset(&idt_entries, 0, sizeof(idt_entry) * IDT_ENTRY_LIMIT);

  // TODO: make this better
  idt_set_entry(0, (uint32_t) _exception0, 0x08, 0x8E);
  idt_set_entry(1, (uint32_t) _exception1, 0x08, 0x8E);
  idt_set_entry(2, (uint32_t) _exception2, 0x08, 0x8E);
  idt_set_entry(3, (uint32_t) _exception3, 0x08, 0x8E);
  idt_set_entry(4, (uint32_t) _exception4, 0x08, 0x8E);
  idt_set_entry(5, (uint32_t) _exception5, 0x08, 0x8E);
  idt_set_entry(6, (uint32_t) _exception6, 0x08, 0x8E);
  idt_set_entry(7, (uint32_t) _exception7, 0x08, 0x8E);
  idt_set_entry(8, (uint32_t) _exception8, 0x08, 0x8E);
  idt_set_entry(9, (uint32_t) _exception9, 0x08, 0x8E);
  idt_set_entry(10, (uint32_t) _exception10, 0x08, 0x8E);
  idt_set_entry(11, (uint32_t) _exception11, 0x08, 0x8E);
  idt_set_entry(12, (uint32_t) _exception12, 0x08, 0x8E);
  idt_set_entry(13, (uint32_t) _exception13, 0x08, 0x8E);
  idt_set_entry(14, (uint32_t) _exception14, 0x08, 0x8E);
  idt_set_entry(15, (uint32_t) _exception15, 0x08, 0x8E);
  idt_set_entry(16, (uint32_t) _exception16, 0x08, 0x8E);
  idt_set_entry(17, (uint32_t) _exception17, 0x08, 0x8E);
  idt_set_entry(18, (uint32_t) _exception18, 0x08, 0x8E);
  idt_set_entry(19, (uint32_t) _exception19, 0x08, 0x8E);
  idt_set_entry(20, (uint32_t) _exception20, 0x08, 0x8E);
  idt_set_entry(21, (uint32_t) _exception21, 0x08, 0x8E);
  idt_set_entry(22, (uint32_t) _exception22, 0x08, 0x8E);
  idt_set_entry(23, (uint32_t) _exception23, 0x08, 0x8E);
  idt_set_entry(24, (uint32_t) _exception24, 0x08, 0x8E);
  idt_set_entry(25, (uint32_t) _exception25, 0x08, 0x8E);
  idt_set_entry(26, (uint32_t) _exception26, 0x08, 0x8E);
  idt_set_entry(27, (uint32_t) _exception27, 0x08, 0x8E);
  idt_set_entry(28, (uint32_t) _exception28, 0x08, 0x8E);
  idt_set_entry(29, (uint32_t) _exception29, 0x08, 0x8E);
  idt_set_entry(30, (uint32_t) _exception30, 0x08, 0x8E);
  idt_set_entry(31, (uint32_t) _exception31, 0x08, 0x8E);

  idt_flush((uint32_t) &idt);

}
