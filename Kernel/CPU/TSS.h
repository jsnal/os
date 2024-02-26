/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

struct [[gnu::packed]] TSS {
    u16 backlink, __backlink;
    u32 esp0;
    u16 ss0, __ss0;
    u32 esp1;
    u16 ss1, __ss1;
    u32 esp2;
    u16 ss2, __ss2;
    u32 cr3, eip, eflags;
    u32 eax, ecx, edx, ebx, esp, ebp, esi, edi;
    u16 es, __es;
    u16 cs, __cs;
    u16 ss, __ss;
    u16 ds, __ds;
    u16 fs, __fs;
    u16 gs, __gs;
    u16 ldt, __ldt;
    u16 trace, iomapbase;
};

inline void load_tss_pointer(u16 seg)
{
    asm volatile("ltr %0"
                 :
                 : "r"(seg));
}
