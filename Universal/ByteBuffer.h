/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Stdlib.h>
#include <Universal/Types.h>

class ByteBuffer {
public:
    ByteBuffer() = default;

    ByteBuffer(size_t size, bool zeroed = false)
        : m_size(size)
    {
        m_data = new u8[m_size];

        if (zeroed) {
            memset(m_data, 0, size);
        }
    }

    ByteBuffer(const ByteBuffer& other)
        : m_size(other.m_size)
    {
        m_data = new u8[m_size];
        if (m_size > 0 && other.m_data != nullptr) {
            memcpy(m_data, other.m_data, m_size);
        }
    }

    ByteBuffer(ByteBuffer&& other)
        : m_size(other.m_size)
        , m_data(other.m_data)
    {
        other.m_size = 0;
        other.m_data = nullptr;
    }

    ~ByteBuffer() { clear(); }

    ByteBuffer& operator=(ByteBuffer const& other)
    {
        if (this != &other) {
            m_size = other.m_size;
            m_data = new u8[m_size];

            if (m_size > 0 && other.m_data != nullptr) {
                memcpy(m_data, other.m_data, m_size);
            }
        }
        return *this;
    }

    ByteBuffer& operator=(ByteBuffer&& other)
    {
        if (this != &other) {
            m_size = other.m_size;
            m_data = other.m_data;
            other.m_size = 0;
            other.m_data = nullptr;
        }
        return *this;
    }

    u8& operator[](size_t index) { return m_data[index]; }
    const u8& operator[](size_t index) const { return m_data[index]; }

    void clear()
    {
        if (m_data != nullptr) {
            delete[] m_data;
            m_data = nullptr;
        }
        m_size = 0;
    }

    constexpr bool is_null() const { return m_data == nullptr; }

    const u8* data() const { return m_data; }
    u8* data() { return m_data; }

    size_t size() const { return m_size; }

private:
    u8* m_data { nullptr };
    size_t m_size { 0 };
};
