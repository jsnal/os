/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibA/Number.h>
#include <LibA/Stdlib.h>
#include <LibA/Types.h>

namespace LibA {

class Bitmap {
public:
    static Bitmap wrap(u8* data, size_t size)
    {
        return Bitmap(data, size);
    }

    Bitmap(u8* data, size_t size)
        : m_data(data)
        , m_size(size)
    {
    }

    size_t size() const { return m_size; }

    bool get(u32 index) const
    {
        return 0 != (m_data[index / 8] & (1u << (index % 8)));
    }

    void set(u32 index, bool value)
    {
        if (value) {
            m_data[index / 8] |= static_cast<u8>(1u << (index % 8));
        } else {
            m_data[index / 8] &= static_cast<u8>(~(1u << (index % 8)));
        }
    }

    void fill(u32 value)
    {
        memset(m_data, value, size_in_bytes());
    }

    u8* data() const { return m_data; }

    u32 size_in_bytes() const { return ceiling_divide(m_size, 8); }

private:
    u8* m_data;
    size_t m_size;
};

}

using LibA::Bitmap;
