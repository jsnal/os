/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Result.h>
#include <Universal/Types.h>

namespace Universal {

// TODO: Make this better suited for storing heap allocated data

template<typename T>
class ArrayList {
public:
    ArrayList(size_t capacity)
        : m_capacity(capacity)
    {
        m_data = new T[capacity];
    }

    ArrayList()
        : ArrayList(1)
    {
    }

    Result add(u32 index, T element)
    {
        if (index < 0 || index > m_size) {
            return Result::Failure;
        }

        enure_capacity(m_size + 1);

        for (int i = m_size - 1; i >= static_cast<int>(index); i--) {
            m_data[i + 1] = m_data[i];
        }

        m_data[index] = element;
        m_size++;
        return Result::OK;
    }

    Result add_first(T element) { return add(0, element); }

    Result add_last(T element) { return add(m_size, element); }

    Result remove(u32 index)
    {
        if (check_index(index)) {
            return Result::Failure;
        }

        for (int i = index; i < static_cast<int>(m_size); i++) {
            m_data[i] = m_data[i + 1];
        }

        m_size--;
        return Result::OK;
    }

    Result set(u32 index, T element)
    {
        if (check_index(index)) {
            return Result::Failure;
        }

        m_data[index] = element;
        return Result::OK;
    }

    const T& get(u32 index) const
    {
        ASSERT(check_index(index));
        return m_data[index];
    }

    T& get(u32 index)
    {
        ASSERT(check_index(index));
        return m_data[index];
    }

    const T& operator[](u32 index) const { return m_data[index]; }
    T& operator[](u32 index) { return m_data[index]; }

    T first() const { return m_data[0]; }
    T last() const { return m_data[m_size - 1]; }

    bool empty() const { return m_size == 0; }

    size_t size() const { return m_size; }

    size_t capacity() const { return m_capacity; }

private:
    bool check_index(u32 index) const { return index < 0 || index >= m_size; }

    void enure_capacity(int minimum_capacity)
    {
        if (minimum_capacity < m_capacity) {
            return;
        }

        m_capacity = (m_capacity * 2) + 1;
        if (m_capacity < minimum_capacity) {
            m_capacity = minimum_capacity;
        }

        T* new_data = new T[m_capacity];

        for (size_t i = 0; i < m_size; i++) {
            new_data[i] = m_data[i];
        }

        delete m_data;
        m_data = new_data;
    }

    T* m_data;
    size_t m_size { 0 };
    size_t m_capacity { 0 };
};
}

using Universal::ArrayList;
