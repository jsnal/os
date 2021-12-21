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

  idt_set_entry(0, (uint32_t) _exception0, 0x08, 0x8E);
  idt_set_entry(1, (uint32_t) _exception1, 0x08, 0x8E);

  idt_flush((uint32_t) &idt);

}
