/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Process/Process.h>
#include <Kernel/Process/Spinlock.h>
#include <Universal/Function.h>
#include <Universal/Result.h>
#include <Universal/SharedPtr.h>
#include <Universal/Types.h>

#define PM ProcessManager::the()

class ProcessManager final {

public:
    static ProcessManager& the();

    ProcessManager();

    static bool started() { return s_started; }

    void start();
    void timer_tick(size_t milliseconds_since_boot);

    Process* current_process_ptr() { return m_current_process; }
    Process& current_process() { return *m_current_process; }
    const Process& current_process() const { return *m_current_process; }

    void add_process(Process&) const;
    void remove_process(Process&) const;

    pid_t get_next_pid() { return m_current_pid++; }

    Process* from_pid(pid_t) const;
    void for_each_child(Process&, Function<bool(Process&)>) const;

    void schedule();

    void yield();

    void enter_critical();
    void exit_critical();

    TSS* tss() { return &m_tss; }

private:
    static bool s_started;
    pid_t m_current_pid { 0 };
    Process* m_current_process { nullptr };
    Process* m_kernel_idle_process { nullptr };
    LinkedList<Process>* m_processes { nullptr };

    TSS m_tss;

    u8 m_critical_count { 0 };
};
