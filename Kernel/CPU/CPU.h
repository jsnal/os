/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Types.h>

namespace CPU {

enum RingLevel : u8 {
    Ring0 = 0,
    Ring1 = 1,
    Ring2 = 2,
    Ring3 = 3,
};

struct [[gnu::packed]] SegmentSelector {
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

static inline void set_ds_register(const SegmentSelector& selector)
{
    asm volatile("mov %%ds, %0"
                 :
                 : "r"(static_cast<uint16_t>(selector)));
}

static inline void set_es_register(const SegmentSelector& selector)
{
    asm volatile("mov %%es, %0"
                 :
                 : "r"(static_cast<uint16_t>(selector)));
}

static inline void set_fs_register(const SegmentSelector& selector)
{
    asm volatile("mov %%fs, %0"
                 :
                 : "r"(static_cast<uint16_t>(selector)));
}

static inline void set_gs_register(const SegmentSelector& selector)
{
    asm volatile("mov %%gs, %0"
                 :
                 : "r"(static_cast<uint16_t>(selector)));
}
};
