/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Process/Process.h>
#include <Universal/Atomic.h>

class Process;

class WaitingStatus {
public:
    WaitingStatus() = default;

    WaitingStatus(Process* process)
        : m_process(process)
    {
    }

    Process& process() { return *m_process; }

    [[gnu::always_inline]] inline void set_waiting() { m_is_waiting.store(1); };
    [[gnu::always_inline]] inline void set_ready() { m_is_waiting.store(0); };

    [[gnu::always_inline]] inline bool is_ready() { return m_is_waiting.load() == 0; };

private:
    Process* m_process;

    Atomic<u8> m_is_waiting { 0 };
};
