/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Process/Process.h>
#include <Universal/Result.h>
#include <Universal/SharedPtr.h>
#include <Universal/Types.h>

class ProcessManager final {

public:
    static ProcessManager& the();

    static void init();

    ProcessManager();

    void create_kernel_process(void (*entry_point)(), const char* name);

    ResultOr<Process*> find_by_pid(pid_t) const;

    void schedule();

private:
    static pid_t get_next_pid() { return s_current_pid++; }

    static pid_t s_current_pid;
    static Process* s_current_process;
    static Process* s_kernel_process;
    static LinkedList<Process>* s_processes;
};
