/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

struct [[gnu::packed]] TSS {
    u16 link;
    u16 link_high;
    u32 esp0;
    u16 ss0;
    u16 ss0_high;
    u32 esp1;
    u16 ss1;
    u16 ss1_high;
    u32 esp2;
    u16 ss2;
    u16 ss2_high;
    u32 cr3;
    u32 eip;
    u32 eflags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u16 es;
    u16 es_high;
    u16 cs;
    u16 cs_high;
    u16 ss;
    u16 ss_high;
    u16 ds;
    u16 ds_high;
    u16 fs;
    u16 fs_high;
    u16 gs;
    u16 gs_high;
    u16 ldt;
    u16 ldt_high;
    u16 iopb_low;
    u16 iopb;
};

inline void tss_load(u16 seg)
{
    asm volatile("ltr %0"
                 :
                 : "r"(seg));
}
