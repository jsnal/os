/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/kprintf.h>
#include <Universal/Types.h>

namespace CPU {

#define panic(format, ...)                                                               \
    do {                                                                                 \
        kprintf("*** PANIC (%s:%d in %s) ***\n", __FUNCTION__, __LINE__, __FILE_NAME__); \
        kprintf(format, ##__VA_ARGS__);                                                  \
        CPU::hang();                                                                     \
    } while (0)

[[noreturn]] void hang();
void assertion_failed(const char* message, const char* file, const char* function, u32 line);

enum RingLevel : u8 {
    Ring0 = 0,
    Ring1 = 1,
    Ring2 = 2,
    Ring3 = 3,
};

class [[gnu::packed]] SegmentSelector {
public:
    SegmentSelector(RingLevel ring, u8 index)
        : m_ring(ring)
        , m_type(0)
        , m_index(index)
    {
    }

    u16 get() const
    {
        return m_ring | m_type << 2 | m_index << 3;
    }

    operator u16() const
    {
        return get();
    }

private:
    u16 m_ring : 2;
    u16 m_type : 1;
    u16 m_index : 13;
};

void sti();
void cli();
void set_ds_register(const SegmentSelector& selector);
void set_es_register(const SegmentSelector& selector);
void set_fs_register(const SegmentSelector& selector);
void set_gs_register(const SegmentSelector& selector);
u32 cpu_flags();

class InterruptDisabler {
public:
    InterruptDisabler()
    {
        m_flags = cpu_flags();
        cli();
    }

    ~InterruptDisabler()
    {
        if (m_flags & 0x0200) {
            sti();
        }
    }

private:
    u32 m_flags { 0 };
};

};
