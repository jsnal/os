/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibA/Types.h>

namespace LibA {

template<typename T, size_t S>
class Array {
public:
    Array();

    Array(const Array& other)
    {
        for (size_t i = 0; i < other.size(); i++) {
            m_data[i] = other[i];
        }
    }

    Array& operator=(const Array&) = delete;
    Array(Array&&) = delete;
    Array& operator=(Array&&) = delete;

    T& data() const { return m_data; }
    constexpr size_t size() const { return S; }

    T& operator[](size_t i) { return m_data[i]; }
    const T& operator[](size_t i) const { return m_data[i]; }

private:
    T m_data[S];
};

}

using LibA::Array;
