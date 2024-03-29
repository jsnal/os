/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Assert.h>
#include <Universal/Stdlib.h>

namespace Universal {

template<typename T>
class SharedPtr {
public:
    SharedPtr() { }

    SharedPtr(const T* ptr)
        : m_ptr(const_cast<T*>(ptr))
    {
        if (ptr != nullptr) {
            m_ptr->ref();
        }
    }

    SharedPtr(const T& object)
        : m_ptr(const_cast<T*>(object))
    {
        m_ptr->ref();
    }

    SharedPtr(bool adopt, T& object)
        : m_ptr(&object)
    {
    }

    SharedPtr(SharedPtr&& other)
        : m_ptr(other.leak_ref())
    {
    }

    T* leak_ref()
    {
        return exchange(m_ptr, nullptr);
    }

    [[gnu::always_inline]] inline ~SharedPtr() { clear(); }

    [[gnu::always_inline]] inline SharedPtr& operator=(SharedPtr&& other)
    {
        SharedPtr p = move(other);
        swap(m_ptr, p.m_ptr);
        return *this;
    }

    [[gnu::always_inline]] inline SharedPtr& operator=(SharedPtr& other)
    {
        SharedPtr p = other;
        swap(m_ptr, p.m_ptr);
        return *this;
    }

    [[gnu::always_inline]] inline SharedPtr& operator=(const T* ptr)
    {
        SharedPtr p = ptr;
        swap(m_ptr, p.m_ptr);
        return *this;
    }

    [[gnu::always_inline]] inline SharedPtr& operator=(const T& object)
    {
        SharedPtr p = object;
        swap(m_ptr, p.m_ptr);
        return *this;
    }

    SharedPtr& operator=(std::nullptr_t)
    {
        clear();
        return *this;
    }

    [[gnu::always_inline]] inline void clear()
    {
        if (m_ptr != nullptr) {
            m_ptr->unref();
        }
        m_ptr = nullptr;
    }

    bool operator!() const { return !m_ptr; }

    [[gnu::always_inline]] inline T* ptr() { return m_ptr; }
    [[gnu::always_inline]] inline const T* ptr() const { return m_ptr; }

    [[gnu::always_inline]] inline T* operator->() { return m_ptr; }
    [[gnu::always_inline]] inline const T* operator->() const { return m_ptr; }

    [[gnu::always_inline]] inline T& operator*() { return *m_ptr; }
    [[gnu::always_inline]] inline const T& operator*() const { return *m_ptr; }

    operator bool() { return !!m_ptr; }

    bool operator==(std::nullptr_t) const { return !m_ptr; }
    bool operator!=(std::nullptr_t) const { return m_ptr; }

    bool operator==(const SharedPtr& other) const { return m_ptr == other.m_ptr; }
    bool operator!=(const SharedPtr& other) const { return m_ptr != other.m_ptr; }

    bool operator==(SharedPtr& other) { return m_ptr == other.m_ptr; }
    bool operator!=(SharedPtr& other) { return m_ptr != other.m_ptr; }

    bool operator==(const T* other) const { return m_ptr == other; }
    bool operator!=(const T* other) const { return m_ptr != other; }

    bool operator==(T* other) { return m_ptr == other; }
    bool operator!=(T* other) { return m_ptr != other; }

    bool is_null() const { return !m_ptr; }

private:
    T* m_ptr;
};

template<typename T>
inline SharedPtr<T> adopt_if_nonnull(T* object)
{
    if (object) {
        return SharedPtr<T>(true, *object);
    }
    return {};
}

template<typename T>
inline SharedPtr<T> adopt(T& object)
{
    return SharedPtr<T>(true, object);
}

}

using Universal::adopt;
using Universal::adopt_if_nonnull;
using Universal::SharedPtr;
