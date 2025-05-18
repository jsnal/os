/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Malloc.h>
#include <Universal/Result.h>
#include <Universal/Types.h>

template<typename T>
class CircularQueue {
public:
    CircularQueue(size_t capacity)
        : m_capacity(capacity)
        , m_size(0)
    {
        m_queue = new T[capacity]();
    }

    CircularQueue(CircularQueue&& other)
        : m_queue(other.m_queue)
        , m_capacity(other.m_capacity)
        , m_size(other.m_size)
        , m_front(other.m_front)
        , m_back(other.m_back)
    {
        other.m_queue = nullptr;
        other.m_capacity = 0;
        other.m_size = 0;
        other.m_front = 0;
        other.m_back = 0;
    }

    CircularQueue& operator=(CircularQueue&& other)
    {
        if (this != &other) {
            delete[] m_queue;

            m_queue = other.m_queue;
            m_capacity = other.m_capacity;
            m_size = other.m_size;
            m_front = other.m_front;
            m_back = other.m_back;

            other.m_queue = nullptr;
            other.m_capacity = 0;
            other.m_size = 0;
            other.m_front = 0;
            other.m_back = 0;
        }
        return *this;
    }

    ~CircularQueue()
    {
        delete[] m_queue;
    }

    size_t capacity() const { return m_capacity; }
    size_t size() const { return m_size; }

    bool is_full() { return m_size == m_capacity; }
    bool is_empty() { return m_size == 0; }

    void enqueue(const T& element)
    {
        if (is_empty()) {
            m_front = 0;
            m_back = 0;
        } else {
            m_back = (m_back + 1) % m_capacity;
        }

        m_queue[m_back] = element;
        m_size++;
    }

    T dequeue()
    {
        ASSERT(!is_empty());

        T element = m_queue[m_front];
        m_size--;

        if (is_empty()) {
            m_front = 0;
            m_back = 0;
        } else {
            m_front = (m_front + 1) % m_capacity;
        }

        return element;
    }

    T& front() { return m_queue[m_front]; }
    const T& front() const { return m_queue[m_front]; }

    T& back() { return m_queue[m_back]; }
    const T& back() const { return m_queue[m_back]; }

private:
    T* m_queue { nullptr };
    size_t m_capacity { 0 };
    size_t m_size { 0 };
    size_t m_front { 0 };
    size_t m_back { 0 };
};
