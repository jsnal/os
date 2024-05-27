/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Assert.h>
#include <Universal/Stdlib.h>

#ifdef KERNEL
#    include <Kernel/kmalloc.h>
#endif

#ifdef TESTING
#    include <new>
#endif

template<typename T>
class Optional {
public:
    Optional()
        : m_has_value(false)
    {
    }

    Optional(const T& value)
        : m_has_value(true)
    {
        new (&m_storage) T(value);
    }

    Optional(T&& value)
        : m_has_value(true)
    {
        new (&m_storage) T(move(value));
    }

    ~Optional()
    {
        reset();
    }

    Optional(const Optional& other)
        : m_has_value(other.m_has_value)
    {
        if (m_has_value) {
            new (&m_storage) T(other.value());
        }
    }

    Optional(Optional&& other)
        : m_has_value(other.m_has_value)
    {
        if (m_has_value) {
            new (&m_storage) T(move(other.value()));
            other.reset();
        }
    }

    Optional& operator=(const Optional& other)
    {
        if (this != &other) {
            reset();
            if (other.m_has_value) {
                new (&m_storage) T(other.value());
                m_has_value = true;
            }
        }
        return *this;
    }

    Optional& operator=(Optional&& other)
    {
        if (this != &other) {
            reset();
            if (other.m_has_value) {
                new (&m_storage) T(move(other.value()));
                m_has_value = true;
                other.reset();
            }
        }
        return *this;
    }

    bool has_value() const { return m_has_value; }

    T& value()
    {
        ASSERT(m_has_value);
        return *reinterpret_cast<T*>(&m_storage);
    }

    const T& value() const
    {
        ASSERT(m_has_value);
        return *reinterpret_cast<const T*>(&m_storage);
    }

    T release_value()
    {
        T released_value = move(value());
        value().~T();
        m_has_value = false;
        return released_value;
    }

    void reset()
    {
        if (m_has_value) {
            reinterpret_cast<T*>(&m_storage)->~T();
            m_has_value = false;
        }
    }

private:
    bool m_has_value { false };

    alignas(T) char m_storage[sizeof(T)];
};
