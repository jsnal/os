/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

// Uses GNU GCC atomic extension: https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html

enum class MemoryOrder {
    Relaxed = __ATOMIC_RELAXED,
    Acquire = __ATOMIC_ACQUIRE,
    Release = __ATOMIC_RELEASE,
    AcqRel = __ATOMIC_ACQ_REL,
    SeqCst = __ATOMIC_SEQ_CST,
    Consume = __ATOMIC_CONSUME,
};

template<typename T, MemoryOrder default_order = MemoryOrder::SeqCst>
class Atomic {
public:
    Atomic() = default;

    Atomic(T value)
        : m_value(value)
    {
    }

    [[gnu::always_inline]] inline T load(MemoryOrder order = default_order) const volatile noexcept
    {
        return __atomic_load_n(&m_value, (int)order);
    }

    [[gnu::always_inline]] inline void store(T value, MemoryOrder order = default_order) volatile noexcept
    {
        __atomic_store_n(&m_value, value, (int)order);
    }

    [[gnu::always_inline]] inline T add(T value, MemoryOrder order = default_order) volatile noexcept
    {
        return __atomic_fetch_add(&m_value, value, (int)order);
    }

    [[gnu::always_inline]] inline T sub(T value, MemoryOrder order = default_order) volatile noexcept
    {
        return __atomic_fetch_sub(&m_value, value, (int)order);
    }

    [[gnu::always_inline]] inline T exchange(T desired, MemoryOrder order = default_order) volatile noexcept
    {
        return __atomic_exchange_n(&m_value, desired, (int)order);
    }

    [[gnu::always_inline]] inline bool compare_exchange_strong(T& expected, T desired, MemoryOrder order = default_order) volatile noexcept
    {
        if (order == MemoryOrder::AcqRel || order == MemoryOrder::Release) {
            return __atomic_compare_exchange(&m_value, &expected, &desired, false, (int)MemoryOrder::Release, (int)MemoryOrder::Acquire);
        }

        return __atomic_compare_exchange(&m_value, &expected, &desired, false, (int)order, (int)order);
    }

private:
    T m_value;
};
