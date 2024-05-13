/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Function.h>
#include <Universal/Result.h>
#include <Universal/Stdlib.h>
#include <Universal/Types.h>

#ifdef KERNEL
#    include <Kernel/kmalloc.h>
#endif

namespace Universal {

template<typename T>
class ArrayList {
public:
    ArrayList(size_t capacity)
        : m_capacity(capacity)
    {
        ensure_capacity(capacity);
    }

    ArrayList()
        : ArrayList(0)
    {
    }

    ArrayList(ArrayList&& other)
        : m_data(other.m_data)
        , m_size(other.m_size)
        , m_capacity(other.m_capacity)

    {
        other.m_data = nullptr;
        other.m_capacity = 0;
        other.m_size = 0;
    }

    ArrayList& operator=(ArrayList&& other)
    {
        if (this != &other) {
            delete[] m_data;

            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;

            other.m_data = nullptr;
            other.m_capacity = 0;
            other.m_size = 0;
        }
        return *this;
    }

    ~ArrayList()
    {
        delete[] m_data;
    }

    Result add(u32 index, T element)
    {
        if (index < 0 || index > m_size) {
            return Result::Failure;
        }

        ensure_capacity(m_size + 1);

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

    // TODO: Replace this with quick-sort
    void sort(Function<bool(T&, T&)> compare)
    {
        bool swapped;
        for (int i = 0; i < m_size - 1; i++) {
            swapped = false;
            for (int j = 0; j < m_size - i - 1; j++) {
                if (compare(m_data[j], m_data[j + 1])) {
                    swap(m_data[j], m_data[j + 1]);
                    swapped = true;
                }
            }

            if (!swapped) {
                break;
            }
        }
    }

    const T& operator[](u32 index) const { return m_data[index]; }
    T& operator[](u32 index) { return m_data[index]; }

    T& first() const { return m_data[0]; }
    T& last() const { return m_data[m_size - 1]; }

    bool empty() const { return m_size == 0; }

    size_t size() const { return m_size; }

    size_t capacity() const { return m_capacity; }

private:
    bool check_index(u32 index) const { return index < 0 || index >= m_size; }

    void ensure_capacity(int minimum_capacity)
    {
        if (minimum_capacity < m_capacity) {
            return;
        }

        m_capacity = (m_capacity * 2) + 1;
        if (m_capacity < minimum_capacity) {
            m_capacity = minimum_capacity;
        }

        T* new_data = new T[m_capacity]();

        for (size_t i = 0; i < m_size; i++) {
            new (&new_data[i]) T(move(m_data[i]));
            m_data[i].~T();
        }

        delete[] m_data;
        m_data = new_data;
    }

    T* m_data { nullptr };
    size_t m_size { 0 };
    size_t m_capacity { 0 };
};
}

using Universal::ArrayList;
