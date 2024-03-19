/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Number.h>
#include <Universal/Result.h>
#include <Universal/Stdlib.h>
#include <Universal/Types.h>

class ByteBuffer {
public:
    static ByteBuffer create(size_t size)
    {
        return ByteBuffer(size);
    }

    static ByteBuffer create_zeroed(size_t size)
    {
        auto buffer = ByteBuffer(size);
        memset(buffer.m_buffer, 0, size);
        return buffer;
    }

    ~ByteBuffer() { clear(); }

    Result append(void const* data, size_t data_size)
    {
        if (data == nullptr || data_size == 0 || m_size + data_size > m_capacity) {
            return Result::Failure;
        }

        // TODO: Add a resize operation

        memcpy(m_buffer + m_size, data, data_size);
        m_size += data_size;
        return Result::OK;
    }

    Result set(size_t index, u8 data)
    {
        if (!number_between_inclusive(index, (size_t)0, m_capacity)) {
            return Result::Failure;
        }

        m_buffer[index] = data;
        return Result::OK;
    }

    void operator+=(ByteBuffer const& other) { append(other.ptr(), other.size()); }

    u8& operator[](size_t index) { return m_buffer[index]; }
    const u8& operator[](size_t index) const { return m_buffer[index]; }

    void clear()
    {
        if (m_buffer != nullptr) {
            delete m_buffer;
            m_buffer = nullptr;
        }
        m_size = 0;
    }

    u8* ptr() { return m_buffer; }
    const u8* ptr() const { return m_buffer; }

    size_t capacity() const { return m_capacity; }
    size_t size() const { return m_size; }

private:
    ByteBuffer(size_t capacity)
        : m_capacity(capacity)
    {
        m_buffer = new u8[capacity];
    }

    u8* m_buffer { nullptr };
    size_t m_capacity { 0 };
    size_t m_size { 0 };
};
