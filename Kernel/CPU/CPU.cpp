/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/CPU/CPU.h>
#include <Universal/Types.h>

namespace CPU {

void hang()
{
    asm volatile("cli; \
                  hlt");

    while (true)
        ;
}

void assertion_failed(const char* message, const char* file, const char* function, u32 line)
{
    kprintf("*** ASSERTION FAILED (%s:%d in %s) ***\n", function, line, file);
    kprintf("%s\n", message);
    hang();
}

void sti()
{
    asm volatile("sti");
}

void cli()
{
    asm volatile("cli");
}

void set_ds_register(const SegmentSelector& selector)
{
    asm volatile("mov %%ds, %0"
                 :
                 : "r"(static_cast<uint16_t>(selector)));
}

void set_es_register(const SegmentSelector& selector)
{
    asm volatile("mov %%es, %0"
                 :
                 : "r"(static_cast<uint16_t>(selector)));
}

void set_fs_register(const SegmentSelector& selector)
{
    asm volatile("mov %%fs, %0"
                 :
                 : "r"(static_cast<uint16_t>(selector)));
}

void set_gs_register(const SegmentSelector& selector)
{
    asm volatile("mov %%gs, %0"
                 :
                 : "r"(static_cast<uint16_t>(selector)));
}

u32 cpu_flags()
{
    u32 flags;
    asm volatile(
        "pushf\n"
        "pop %0\n"
        : "=rm"(flags)
        :
        : "memory");
    return flags;
}
}
