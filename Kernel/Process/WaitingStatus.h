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

    Process* process() { return m_process; }

    void set_waiting();
    void set_ready();

    bool is_waiting() const;
    bool is_ready() const;

private:
    Process* m_process;

    bool m_is_waiting { true };
};
