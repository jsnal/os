#pragma once

#include <stdint.h>

#define GDT_ENTRY_LIMIT 3

struct [[gnu::packed]] GDTEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
};

struct [[gnu::packed]] GDTPointer {
    uint16_t limit;
    uint32_t base;
};

extern "C" void gdt_load(uint32_t base);

namespace GDT {

void init();

}
