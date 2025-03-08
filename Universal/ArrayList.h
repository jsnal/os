/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Function.h>
#include <Universal/Malloc.h>
#include <Universal/Result.h>
#include <Universal/Stdlib.h>
#include <Universal/Types.h>

namespace Universal {

template<typename T, size_t InlineCapacity = 0>
class ArrayList {
public:
    ArrayList()
        : m_capacity(InlineCapacity)
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
            clear();

            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;

            other.m_data = nullptr;
            other.m_capacity = 0;
            other.m_size = 0;
        }
        return *this;
    }

    ~ArrayList() { clear(); }

    void clear()
    {
        for (size_t i = 0; i < m_size; i++) {
            data()[i].~T();
        }
        m_size = 0;

        if (!is_inlined()) {
            delete[] m_data;
            m_data = nullptr;
        }
        m_capacity = InlineCapacity;
    }

    Result add(u32 index, T&& element)
    {
        if (index < 0 || index > m_size) {
            return Result::Failure;
        }

        ensure_capacity(m_size + 1);
        m_size++;

        for (int i = m_size - 1; i > static_cast<int>(index); i--) {
            new (&data()[i]) T(move(data()[i - 1]));
            data()[i - 1].~T();
        }

        new (&data()[index]) T(move(element));
        return Result::OK;
    }
    Result add(u32 index, const T& element) { return add(index, T(element)); }

    Result add_first(T&& element) { return add(0, move(element)); }
    Result add_first(const T& element) { return add(0, element); }

    Result add_last(T&& element) { return add(m_size, move(element)); }
    Result add_last(const T& element) { return add(m_size, element); }

    Result remove(u32 index)
    {
        if (!is_index_valid(index)) {
            return Result::Failure;
        }

        data()[index].~T();
        for (int i = index + 1; i < static_cast<int>(m_size); i++) {
            new (&data()[i - 1]) T(move(data()[i]));
            data()[i].~T();
        }

        m_size--;
        return Result::OK;
    }

    Result set(u32 index, T element)
    {
        if (!is_index_valid(index)) {
            return Result::Failure;
        }

        data()[index] = element;
        return Result::OK;
    }

    const T& get(u32 index) const
    {
        ASSERT(is_index_valid(index));
        return data()[index];
    }

    T& get(u32 index)
    {
        ASSERT(is_index_valid(index));
        return data()[index];
    }

    ResultReturn<size_t> find(Function<bool(T&)> predicate)
    {
        for (int i = 0; i < m_size; i++) {
            if (predicate(data()[i])) {
                return i;
            }
        }
        return Result::Failure;
    }

    // TODO: Replace this with quick-sort
    void sort(Function<bool(T&, T&)> compare)
    {
        bool swapped;
        for (int i = 0; i < m_size - 1; i++) {
            swapped = false;
            for (int j = 0; j < m_size - i - 1; j++) {
                if (compare(data()[j], data()[j + 1])) {
                    swap(data()[j], data()[j + 1]);
                    swapped = true;
                }
            }

            if (!swapped) {
                break;
            }
        }
    }

    const T& operator[](u32 index) const { return data()[index]; }
    T& operator[](u32 index) { return data()[index]; }

    const T* data() const { return is_inlined() ? inline_data() : m_data; }
    T* data() { return is_inlined() ? inline_data() : m_data; }

    T* raw_data() { return m_data; }

    const T& first() const { return data()[0]; }
    T& first() { return data()[0]; }
    const T& last() const { return data()[m_size - 1]; }
    T& last() { return data()[m_size - 1]; }

    bool is_empty() const { return m_size == 0; }

    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }

private:
    bool is_index_valid(u32 index) const { return index >= 0 && index < m_size; }

    constexpr bool is_inlined() const { return InlineCapacity > 0; }

    T* inline_data() { return reinterpret_cast<T*>(m_inline_data); }
    const T* inline_data() const { return reinterpret_cast<const T*>(m_inline_data); }

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
            new (&new_data[i]) T(move(data()[i]));
            data()[i].~T();
        }

        if (m_data != nullptr) {
            delete[] m_data;
        }
        m_data = new_data;
    }

    T* m_data { nullptr };
    alignas(T) u8 m_inline_data[sizeof(T) * InlineCapacity] {};
    size_t m_size { 0 };
    size_t m_capacity { 0 };
};
}

using Universal::ArrayList;
