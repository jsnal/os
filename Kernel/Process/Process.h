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
#include <Kernel/User.h>
#include <Universal/LinkedList.h>
#include <Universal/Result.h>
#include <Universal/ShareCounted.h>
#include <Universal/SharedPtr.h>
#include <Universal/String.h>
#include <Universal/Types.h>

class WaitingStatus;
class PageDirectory;

extern "C" {
void start_kernel_process(u32* old_stack_pointer);
void start_user_process(u32* old_stack_pointer);
void do_context_switch(u32** old_stack_pointer, u32* new_stack_pointer, u32 cr3);
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
        Dead,
    };

    ~Process();

    static ResultReturn<Process*> create_kernel_process(const String& name, void (*entry_point)(), bool add_to_process_list = true);
    static Result create_user_process(const String& path, uid_t, gid_t);

    ResultReturn<VirtualRegion*> allocate_region(size_t size, u8 access);
    ResultReturn<VirtualRegion*> allocate_region_at(VirtualAddress, size_t size, u8 access);
    Result deallocate_region(size_t index);

    void context_switch(Process*);

    void dump_stack(bool kernel) const;

    const String& name() const { return m_name; }
    pid_t pid() const { return m_pid; }

    bool is_kernel() const { return m_is_kernel; }

    const PageDirectory& page_directory() const { return *m_page_directory; }
    PageDirectory& page_directory() { return *m_page_directory; }
    u32 cr3() const { return m_page_directory->base(); }

    u32* previous_stack_pointer() const { return m_previous_stack_pointer; }

    void set_state(State state) { m_state = state; }
    State state() const { return m_state; }

    void set_ready();
    void set_waiting(WaitingStatus&);

    void reap();

    void sys_exit(int status);
    ssize_t sys_write(int fd, const void* buf, size_t count);
    uid_t sys_getuid();

private:
    Process(const String& name, pid_t, uid_t, gid_t, bool is_kernel);

    static void new_process_runnable();

    Result switch_to_user_mode();
    Result load_elf();

    Result initialize_kernel_stack();
    Result initialize_user_stack();

    String m_name;
    pid_t m_pid { 0 };
    User m_user;

    bool m_is_kernel { false };

    SharedPtr<PageDirectory> m_page_directory;

    // TODO: Make sure this memory is being freed!
    ArrayList<VirtualRegion*> m_regions;

    UniquePtr<VirtualRegion> m_kernel_stack { nullptr };
    UniquePtr<VirtualRegion> m_user_stack { nullptr };
    u32* m_previous_stack_pointer { nullptr };

    void (*m_entry_point)() { nullptr };

    State m_state;

    Process* m_next { nullptr };
    Process* m_previous { nullptr };
};
