#include <Kernel/CPU/GDT.h>
#include <Universal/Logger.h>
#include <Universal/Stdlib.h>

static GDTEntry s_gdt_entries[GDT_ENTRY_LIMIT];
static GDTPointer s_gdt_pointer;

static void gdt_set_entry(u32 num, u32 base, u32 limit, u8 access, u8 gran)
{
    s_gdt_entries[num].base_low = (base & 0xFFFF);
    s_gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    s_gdt_entries[num].base_high = (base >> 24) & 0xFF;

    s_gdt_entries[num].limit_low = (limit & 0xFFFF);
    s_gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    s_gdt_entries[num].granularity |= gran & 0xF0;
    s_gdt_entries[num].access = access;
}

namespace GDT {

void write_tss(TSS* tss)
{
    u32 tss_base_pointer = (u32)tss;
    gdt_set_entry(5, tss_base_pointer, tss_base_pointer + sizeof(TSS) - 1, 0x89, 0x00);
    tss->iopb = sizeof(TSS);
    tss_load(0x28);
}

void init()
{
    s_gdt_pointer.limit = (sizeof(GDTEntry) * GDT_ENTRY_LIMIT) - 1;
    s_gdt_pointer.base = (uintptr_t)&s_gdt_entries;

    gdt_set_entry(0, 0, 0, 0, 0);                // Null segment
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Kernel mode code segment
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Kernel mode data segment
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

    gdt_load((uintptr_t)&s_gdt_pointer);

    dbgprintf("GDT", "Initialized GDT: 0x%x\n", &s_gdt_pointer);
}

}
