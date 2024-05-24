/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/CPU/TSS.h>
#include <Kernel/Memory/Types.h>
#include <Kernel/Process/WaitingStatus.h>
#include <Universal/LinkedList.h>
#include <Universal/Result.h>
#include <Universal/ShareCounted.h>
#include <Universal/SharedPtr.h>
#include <Universal/String.h>
#include <Universal/Types.h>

class WaitingStatus;
class PageDirectory;

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

    static ResultReturn<Process*> create_kernel_process(const String& name, void (*entry_point)(), bool add_to_process_list = true);
    static Result create_user_process(const String& path, uid_t, gid_t);

    void dump_stack() const;

    const String& name() const { return m_name; }
    pid_t pid() const { return m_pid; }

    const PageDirectory& page_directory() const { return *m_page_directory; }

    Context* context() { return m_context; }
    Context** context_ptr() { return &m_context; }

    void set_state(State state) { m_state = state; }
    State state() const { return m_state; }

    void set_ready();
    void set_waiting(WaitingStatus&);

private:
    Process(const String& name, pid_t, uid_t, gid_t, bool is_kernel);

    Result initialize_stack();

    String m_name;
    pid_t m_pid { 0 };
    uid_t m_uid { 0 };
    gid_t m_gid { 0 };

    bool m_is_kernel { false };

    SharedPtr<PageDirectory> m_page_directory;

    Context* m_context;
    State m_state;

    Process* m_next { nullptr };
    Process* m_previous { nullptr };
};
