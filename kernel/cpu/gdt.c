#include <cpu/gdt.h>

static gdt_entry gdt_entries[GDT_ENTRY_LIMIT];
static gdt_pointer gdt;

static void gdt_set_entry(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
  gdt_entries[num].base_low = (base & 0xFFFF);
  gdt_entries[num].base_middle = (base >> 16) & 0xFF;
  gdt_entries[num].base_high = (base >> 24) & 0xFF;

  gdt_entries[num].limit_low = (limit & 0xFFFF);
  gdt_entries[num].granularity = (limit >> 16) & 0x0F;

  gdt_entries[num].granularity |= gran & 0xF0;
  gdt_entries[num].access = access;
}

void gdt_init()
{
  gdt.limit = (sizeof(gdt_pointer) * GDT_ENTRY_LIMIT) - 1;
  gdt.base = (uint32_t) &gdt_entries;

  gdt_set_entry(0, 0, 0, 0, 0);
  gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
  gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
  gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
  gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
  gdt_flush((uint32_t) &gdt);
}

