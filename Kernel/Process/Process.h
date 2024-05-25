/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/CPU/TSS.h>
#include <Kernel/Memory/Types.h>
#include <Kernel/Memory/VirtualRegion.h>
#include <Kernel/Process/WaitingStatus.h>
#include <Universal/LinkedList.h>
#include <Universal/Result.h>
#include <Universal/ShareCounted.h>
#include <Universal/SharedPtr.h>
#include <Universal/String.h>
#include <Universal/Types.h>

class WaitingStatus;
class PageDirectory;

extern "C" {
void start_kernel_process(u32* old_stack_pointer, u32 cr3);
void start_user_process(u32* old_stack_pointer);
}

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
        //        u32 m_ss { 0 };
        u32 m_edi { 0 };
        u32 m_esi { 0 };
        u32 m_ebx { 0 };
        u32 m_ebp { 0 };
        u32 m_eip { 0 };
        u32 m_cs { 0 };
        u32 m_eflags { 0 };
    };

    static ResultReturn<Process*> create_kernel_process(const String& name, void (*entry_point)(), bool add_to_process_list = true);
    static Result create_user_process(const String& path, void (*entry_point)(), uid_t, gid_t);

    VirtualRegion* allocate_region(VirtualAddress, size_t size, u8 access);

    void dump_stack(bool kernel) const;

    const String& name() const { return m_name; }
    pid_t pid() const { return m_pid; }

    bool is_kernel() const { return m_is_kernel; }

    const PageDirectory& page_directory() const { return *m_page_directory; }
    PageDirectory& page_directory() { return *m_page_directory; }

    u32* previous_stack_pointer() const { return m_previous_stack_pointer; }

    Context* context() { return m_context; }
    Context** context_ptr() { return &m_context; }

    void set_state(State state) { m_state = state; }
    State state() const { return m_state; }

    void set_ready();
    void set_waiting(WaitingStatus&);

private:
    Process(const String& name, pid_t, uid_t, gid_t, bool is_kernel);

    static void new_process_runnable();

    void switch_to_user_mode();

    Result initialize_kernel_stack();
    Result initialize_user_stack();

    String m_name;
    pid_t m_pid { 0 };
    uid_t m_uid { 0 };
    gid_t m_gid { 0 };

    bool m_is_kernel { false };

    SharedPtr<PageDirectory> m_page_directory;

    // TODO: Make sure this memory is being freed!
    ArrayList<VirtualRegion*> m_regions;

    UniquePtr<VirtualRegion> m_kernel_stack { nullptr };
    UniquePtr<VirtualRegion> m_user_stack { nullptr };
    u32* m_previous_stack_pointer { nullptr };

    void (*m_entry_point)() { nullptr };

    Context* m_context;
    State m_state;

    Process* m_next { nullptr };
    Process* m_previous { nullptr };
};
