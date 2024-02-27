/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Process/Process.h>
#include <Universal/SharedPtr.h>
#include <Universal/Types.h>

class ProcessManager final {

public:
    static ProcessManager& the();

    static void init(void (*entry_point)());

    void create_kernel_process(void (*entry_point)(), const char* name);

    void schedule();

    ProcessManager() {};

private:
    static u32 get_next_pid() { return s_current_pid++; }

    static u32 s_current_pid;
    static Process* s_current_process;
    static Process* s_kernel_process;
    static LinkedList<Process>* s_processes;
};
