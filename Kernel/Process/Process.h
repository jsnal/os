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

    struct [[gnu::packed]] Context {
        u32 m_edi { 0 };
        u32 m_esi { 0 };
        u32 m_ebx { 0 };
        u32 m_ebp { 0 };
        u32 m_eip { 0 };
        u32 m_cs { 0 };
        u32 m_eflags { 0 };
    };

    u32 pid() const { return m_pid; }
    const char* name() const { return m_name; }

    u32 esp() { return reinterpret_cast<u32>(m_context); }
    u32* esp_ptr() { return reinterpret_cast<u32*>(m_context); }

    Context* context() { return m_context; }

private:
    Process(void (*entry_point)(), u32 pid, const char* name, bool is_kernel);
    Process(void (*entry_point)(), u32 pid, const char* name, size_t stack_size, bool is_kernel);

    u32 m_pid;
    const char* m_name;
    void (*m_entry_point)() { nullptr };
    bool m_is_kernel { false };
    Context* m_context;

    Process* m_next { nullptr };
    Process* m_previous { nullptr };
};
