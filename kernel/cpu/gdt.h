#ifndef CPU_GDT_H
#define CPU_GDT_H

#include <stdint.h>

#define GDT_ENTRY_LIMIT 5

struct gdt_entry_struct {
  uint16_t limit_low;  // Lower 16 bits of limit
  uint16_t base_low;   // Lower 16 bits of base
  uint8_t base_middle; // Next 8 bits of the base
  uint8_t access;      // Access falgs, determine what rings the segment can be used in
  uint8_t granularity; // Size the limit is scaled by
  uint8_t base_high;   // Last 8 bits of base
} __attribute__ ((packed));

typedef struct gdt_entry_struct gdt_entry;

struct gdt_pointer_struct {
  uint16_t limit; // Upper 16 bits of all selector limits
  uint32_t base;  // Address of the first gdt entry
} __attribute__ ((packed));

typedef struct gdt_pointer_struct gdt_pointer;

void gdt_init();

void gdt_flush(uint32_t base);

void gdt_reload();

#endif
