/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Atomic.h>
#include <Universal/Types.h>

class Spinlock {
public:
    Spinlock() { }

    [[gnu::always_inline]] inline void lock()
    {
        while (m_lock.exchange(1, MemoryOrder::Acquire)) {
            asm volatile("pause");
        }
    }

    [[gnu::always_inline]] inline void unlock()
    {
        m_lock.store(0, MemoryOrder::Release);
    }

private:
    Spinlock(const Spinlock&) = delete;
    Spinlock& operator=(const Spinlock&) = delete;

    Spinlock(Spinlock&&) = delete;
    Spinlock& operator=(Spinlock&&) = delete;

    Atomic<u8> m_lock { 0 };
};
