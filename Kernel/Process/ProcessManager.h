/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Process/Process.h>
#include <Kernel/Process/Spinlock.h>
#include <Universal/Result.h>
#include <Universal/SharedPtr.h>
#include <Universal/Types.h>

#define PM ProcessManager::the()

class ProcessManager final {

public:
    static ProcessManager& the();

    void start();

    Process* current_process() { return m_current_process; }

    ProcessManager();

    void add_process(Process&);

    pid_t get_next_pid() { return m_current_pid++; }

    ResultOr<Process*> find_by_pid(pid_t) const;

    void schedule();

    void yield();

    void enter_critical();
    void exit_critical();

private:
    pid_t m_current_pid { 0 };
    Process* m_current_process { nullptr };
    Process* m_kernel_idle_process { nullptr };
    LinkedList<Process>* m_processes { nullptr };

    u8 m_critical_count { 0 };
};
