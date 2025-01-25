/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Number.h>
#include <Universal/Stdlib.h>
#include <Universal/Types.h>

namespace Universal {

class Bitmap {
public:
    static Bitmap wrap(u8* data, size_t size)
    {
        return Bitmap(data, size);
    }

    static Bitmap create()
    {
        return Bitmap();
    }

    explicit Bitmap()
        : m_size_in_bits(0)
    {
        m_data = nullptr;
    }

    Bitmap(u8* data, size_t size_in_bits)
        : m_data(data)
        , m_size_in_bits(size_in_bits)
    {
    }

    size_t size() const { return m_size_in_bits; }

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

    u32 size_in_bytes() const { return ceiling_divide(m_size_in_bits, 8); }

private:
    u8* m_data;
    size_t m_size_in_bits;
};

inline constexpr size_t calculate_minimum_bytes(size_t data_size)
{
    size_t bytes = 1;
    while (bytes * 8 < data_size) {
        bytes++;
    }
    return bytes;
}

}

using Universal::Bitmap;
using Universal::calculate_minimum_bytes;
