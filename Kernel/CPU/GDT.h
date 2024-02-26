#pragma once

#include <Kernel/CPU/TSS.h>
#include <Universal/Types.h>

#define GDT_ENTRY_LIMIT 6

struct [[gnu::packed]] GDTEntry {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
};

struct [[gnu::packed]] GDTPointer {
    u16 limit;
    u32 base;
};

extern "C" void gdt_load(u32 base);

namespace GDT {

void write_tss(TSS* tss, u16 ss0, u32 esp0);

void init();

}
