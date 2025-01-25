/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Stdlib.h>

namespace Universal {

template<typename T>
class UniquePtr {
public:
    UniquePtr() { }

    UniquePtr(T* ptr)
        : m_ptr(ptr)
    {
    }

    UniquePtr(UniquePtr&& other)
        : m_ptr(other.leak_ptr())
    {
    }

    template<typename U>
    UniquePtr(UniquePtr<U>&& other)
        : m_ptr(static_cast<T*>(other.leak_ptr()))
    {
    }

    UniquePtr(std::nullptr_t) {};

    ~UniquePtr()
    {
        clear();
    }

    UniquePtr& operator=(UniquePtr&& other)
    {
        if (this != &other) {
            delete m_ptr;
            m_ptr = other.leak_ptr();
        }
        return *this;
    }

    template<typename U>
    UniquePtr& operator=(UniquePtr<U>&& other)
    {
        if (this != static_cast<void*>(&other)) {
            delete m_ptr;
            m_ptr = other.leak_ptr();
        }
        return *this;
    }

    UniquePtr& operator=(T* ptr)
    {
        if (m_ptr != ptr) {
            delete m_ptr;
        }
        m_ptr = ptr;
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t)
    {
        clear();
        return *this;
    }

    bool operator!() const { return !m_ptr; }

    T* ptr() { return m_ptr; }
    const T* ptr() const { return m_ptr; }

    T* operator->() { return m_ptr; }
    const T* operator->() const { return m_ptr; }

    T& operator*() { return *m_ptr; }
    const T& operator*() const { return *m_ptr; }

    operator bool() { return !!m_ptr; }

    T* leak_ptr()
    {
        T* ptr = m_ptr;
        m_ptr = nullptr;
        return ptr;
    }

    void clear()
    {
        delete m_ptr;
        m_ptr = nullptr;
    }

private:
    T* m_ptr { nullptr };
};

template<class T, class... Arguments>
inline UniquePtr<T> make_unique_ptr(Arguments&&... arguments)
{
    return UniquePtr<T>(new T(forward<Arguments>(arguments)...));
}

}

using Universal::make_unique_ptr;
using Universal::UniquePtr;
