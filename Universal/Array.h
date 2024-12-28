/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Stdlib.h>
#include <Universal/Types.h>

namespace Universal {

template<typename T, size_t InlineCapacity = 0>
class Array {
public:
    Array()
        : m_size(InlineCapacity)
    {
    }

    Array(size_t size)
        : m_size(size)
    {
        if (!is_inlined()) {
            m_data = new T[size];
        }
    }

    Array(const Array& other)
        : m_size(other.m_size)
    {
        if (!is_inlined()) {
            m_data = new T[m_size];
        }

        for (size_t i = 0; i < m_size; i++) {
            data()[i] = other.data()[i];
        }
    }

    Array(Array&& other)
        : m_data(other.m_data)
        , m_size(other.m_size)
    {
        if (is_inlined()) {
            for (size_t i = 0; i < m_size; i++) {
                new (&inline_data()[i]) T(move(other.inline_data()[i]));
                other.inline_data()[i].~T();
            }
        }
        other.m_data = nullptr;
        other.m_size = 0;
    }

    ~Array() { clear(); }

    Array& operator=(const Array& other)
    {
        if (this != &other) {
            clear();
            m_size = other.m_size;
            if (!is_inlined()) {
                m_data = new T[m_size];
            }

            for (size_t i = 0; i < m_size; i++) {
                data()[i] = other.data()[i];
            }
        }
        return *this;
    }

    Array& operator=(Array&&) = delete;

    const T* data() const { return is_inlined() ? inline_data() : m_data; }
    T* data() { return is_inlined() ? inline_data() : m_data; }

    const u8* ptr() const { return reinterpret_cast<u8*>(data()); }
    u8* ptr() { return reinterpret_cast<u8*>(data()); }

    constexpr size_t size() const { return m_size; }

    const T& operator[](size_t i) const { return data()[i]; }
    T& operator[](size_t i) { return data()[i]; }

private:
    void clear()
    {
        for (size_t i = 0; i < m_size; i++) {
            data()[i].~T();
        }
        m_size = 0;

        if (m_data != nullptr) {
            delete[] m_data;
            m_data = nullptr;
        }
    }

    constexpr bool is_inlined() const { return InlineCapacity > 0; }

    T* inline_data() { return reinterpret_cast<T*>(m_inline_data); }
    const T* inline_data() const { return reinterpret_cast<const T*>(m_inline_data); }

    T* m_data { nullptr };
    alignas(T) u8 m_inline_data[sizeof(T) * InlineCapacity];
    size_t m_size { 0 };
};

}

using Universal::Array;
