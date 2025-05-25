/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Malloc.h>
#include <Universal/Result.h>
#include <Universal/Types.h>

namespace Universal {

template<typename T, size_t Capacity>
class CircularQueue {
public:
    CircularQueue()
    {
        m_data = new T[Capacity]();
    }

    CircularQueue(const CircularQueue<T, Capacity>& other)
        : m_size(other.m_size)
        , m_front(other.m_front)
        , m_back(other.m_back)
    {
        m_data = new T[Capacity]();
        for (size_t i = 0; i < Capacity; i++) {
            m_data[i] = other.m_data[i];
        }
    }

    CircularQueue(CircularQueue<T, Capacity>&& other)
        : m_data(other.m_data)
        , m_size(other.m_size)
        , m_front(other.m_front)
        , m_back(other.m_back)
    {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_front = 0;
        other.m_back = 0;
    }

    ~CircularQueue() { clear(); }

    CircularQueue& operator=(CircularQueue<T, Capacity>& other)
    {
        if (this != &other) {
            m_size = other.m_size;
            m_front = other.m_front;
            m_back = other.m_back;

            m_data = new T[Capacity]();
            for (size_t i = 0; i < Capacity; i++) {
                m_data[i] = other.m_data[i];
            }
        }
        return *this;
    }

    CircularQueue& operator=(CircularQueue<T, Capacity>&& other)
    {
        if (this != &other) {
            clear();

            m_data = other.m_data;
            m_size = other.m_size;
            m_front = other.m_front;
            m_back = other.m_back;

            other.m_data = nullptr;
            other.m_size = 0;
            other.m_front = 0;
            other.m_back = 0;
        }
        return *this;
    }

    size_t capacity() const { return Capacity; }
    size_t size() const { return m_size; }

    bool is_full() const { return m_size == Capacity; }
    bool is_empty() const { return m_size == 0; }

    void enqueue(const T& element)
    {
        if (is_empty()) {
            m_front = 0;
            m_back = 0;
        } else {
            m_back = (m_back + 1) % Capacity;
        }

        m_data[m_back] = element;
        m_size++;
    }

    T dequeue()
    {
        ASSERT(!is_empty());

        T element = m_data[m_front];
        m_size--;

        if (is_empty()) {
            m_front = 0;
            m_back = 0;
        } else {
            m_front = (m_front + 1) % Capacity;
        }

        return element;
    }

    const T& front() const { return m_data[m_front]; }
    T& front() { return m_data[m_front]; }

    const T& back() const { return m_data[m_back]; }
    T& back() { return m_data[m_back]; }

    void clear()
    {
        while (!is_empty()) {
            dequeue().~T();
        }
        delete[] m_data;
    }

    T* data() { return m_data; }

private:
    T* m_data { nullptr };
    size_t m_size { 0 };
    size_t m_front { 0 };
    size_t m_back { 0 };
};

}

using Universal::CircularQueue;
