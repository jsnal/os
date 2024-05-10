/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

namespace Universal {

template<typename T>
class Array {
public:
    Array(size_t size)
        : m_size(size)
    {
        m_data = new T[size];
    }

    Array(const Array& other)
    {
        for (size_t i = 0; i < other.size(); i++) {
            m_data[i] = other[i];
        }
    }

    ~Array()
    {
        delete[] m_data;
    }

    Array& operator=(const Array&) = delete;
    Array(Array&&) = delete;
    Array& operator=(Array&&) = delete;

    T& data() const { return m_data; }
    constexpr size_t size() const { return m_size; }

    T& operator[](size_t i) { return m_data[i]; }
    const T& operator[](size_t i) const { return m_data[i]; }

private:
    T* m_data;
    size_t m_size { 0 };
};

}

using Universal::Array;
