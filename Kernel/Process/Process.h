/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/CPU/TSS.h>
#include <Kernel/Devices/TTYDevice.h>
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

// TODO: Each process should have a priority where some processes get more time
//       and others get less time. Probably a range from 50 ms to 20 ms.
#define QUANTUM_IN_MILLISECONDS 20

class WaitingStatus;
class PageDirectory;

extern "C" {
void start_kernel_process(u32* old_stack_pointer);
void start_user_process();
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
    static Result create_user_process(const String& path, uid_t, gid_t, TTYDevice*);
    static ResultReturn<Process*> fork_user_process(Process& parent, SyscallRegisters& frame);

    ResultReturn<VirtualRegion*> allocate_region(size_t size, u8 access);
    ResultReturn<VirtualRegion*> allocate_region_at(VirtualAddress, size_t size, u8 access);
    Result deallocate_region(size_t index);

    bool timer_expired() { return --m_ticks_left == 0; }
    void reset_timer_ticks() { m_ticks_left = QUANTUM_IN_MILLISECONDS; }
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
    pid_t sys_fork(SyscallRegisters&);
    int sys_ioctl(int fd, uint32_t request, uint32_t* argp);
    ssize_t sys_write(int fd, const void* buf, size_t count);
    ssize_t sys_read(int fd, void* buf, size_t count);
    pid_t sys_getpid();
    uid_t sys_getuid();
    int sys_isatty(int fd);

private:
    static constexpr size_t kKernelStackSize = 16 * KB;
    static constexpr size_t kUserStackSize = 16 * KB;

    Process(const String& name, pid_t, uid_t, gid_t, bool is_kernel, TTYDevice* = nullptr);
    Process(const Process& parent);

    static void prepare_user_process();

    Result load_elf();

    Result initialize_kernel_stack(const SyscallRegisters&);
    Result initialize_user_stack();

    bool is_address_accessible(VirtualAddress);
    ResultReturn<SharedPtr<FileDescriptor>> find_file_descriptor(int fd);

    u8 m_ticks_left { 0 };

    String m_name;
    pid_t m_pid { 0 };
    User m_user;

    bool m_is_kernel { false };

    SharedPtr<TTYDevice> m_tty;

    SharedPtr<PageDirectory> m_page_directory;

    // TODO: Make sure this memory is being freed!
    ArrayList<VirtualRegion*> m_regions;
    ArrayList<SharedPtr<FileDescriptor>> m_fds;

    UniquePtr<VirtualRegion> m_kernel_stack { nullptr };
    UniquePtr<VirtualRegion> m_user_stack { nullptr };
    u32* m_previous_stack_pointer { nullptr };

    void (*m_entry_point)() { nullptr };

    State m_state;

    Process* m_next { nullptr };
    Process* m_previous { nullptr };
};
