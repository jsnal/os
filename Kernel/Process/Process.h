/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/CPU/TSS.h>
#include <Universal/LinkedList.h>
#include <Universal/ShareCounted.h>
#include <Universal/Types.h>

class Process : public LinkedListNode<Process> {
    friend class LinkedListNode<Process>;
    friend class ProcessManager;

public:
    enum State : u8 {
        Created = 0,
        Ready,
        Waiting,
        Running,
        Terminated
    };

    u32 pid() const { return m_pid; }
    const char* name() const { return m_name; }

    u32 ebp() const { return m_ebp; }
    u32 esp() const { return m_esp; }
    u32 eip() const { return m_eip; }

private:
    Process(void (*entry_point)(), u32 pid, const char* name, bool is_kernel);
    Process(void (*entry_point)(), u32 pid, const char* name, size_t stack_size, bool is_kernel);

    u32 m_pid;
    const char* m_name;
    void (*m_entry_point)() { nullptr };
    bool m_is_kernel { false };
    u32 m_ebp { 0 };
    u32 m_esp { 0 };
    u32 m_eip { 0 };
    // TSS m_tss;

    Process* m_next { nullptr };
    Process* m_previous { nullptr };
};
