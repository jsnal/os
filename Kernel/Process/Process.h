/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/CPU/TSS.h>
#include <Kernel/Devices/TTYDevice.h>
#include <Kernel/Memory/PagingTypes.h>
#include <Kernel/Memory/VirtualRegion.h>
#include <Kernel/Process/Blocker.h>
#include <Kernel/Process/WaitingStatus.h>
#include <Kernel/User.h>
#include <LibC/sys/syscall_defines.h>
#include <Universal/BasicString.h>
#include <Universal/Function.h>
#include <Universal/LinkedList.h>
#include <Universal/Result.h>
#include <Universal/SharedPtr.h>
#include <Universal/Types.h>

// TODO: Each process should have a priority where some processes get more time
//       and others get less time. Probably a range from 50 ms to 20 ms.
#define QUANTUM_IN_MILLISECONDS 20

class WaitingStatus;
class PageDirectory;

extern "C" {
void start_first_process(u32* old_stack_pointer);
void first_context_switch();
void context_switch(u32** old_stack_pointer, u32* new_stack_pointer, u32 cr3);
}

class Process : public LinkedListNode<Process> {
    friend class LinkedListNode<Process>;
    friend class ProcessManager;

public:
    enum State : u8 {
        Runnable = 0,
        Running,
        Waiting,
        Blocked,
        Dead,
    };

    ~Process();

    static ResultReturn<Process*> create_kernel_process(StringView name, void (*entry_point)(), bool add_to_process_list = true);
    static ResultReturn<Process*> create_user_process(StringView path, pid_t pid, pid_t ppid, ArrayList<StringView>&& argv, TTYDevice*);
    static ResultReturn<Process*> fork_user_process(Process& parent, TaskRegisters& frame);

    ResultReturn<VirtualRegion*> allocate_region(size_t size, u8 access);
    ResultReturn<VirtualRegion*> allocate_region_at(VirtualAddress, size_t size, u8 access);
    Result deallocate_region(size_t index);

    bool timer_expired() { return --m_ticks_left == 0; }
    void reset_timer_ticks() { m_ticks_left = QUANTUM_IN_MILLISECONDS; }
    void context_switch(Process*);

    void dump_stack(bool kernel) const;

    const String& name() const { return m_name; }
    pid_t pid() const { return m_pid; }
    pid_t ppid() const { return m_ppid; }

    bool is_kernel() const { return m_is_kernel; }
    bool is_dead() const { return m_state == Dead; }

    const PageDirectory& page_directory() const { return *m_page_directory; }
    PageDirectory& page_directory() { return *m_page_directory; }
    u32 cr3() const { return m_page_directory->base(); }

    u32* previous_stack_pointer() const { return m_previous_stack_pointer; }

    void set_state(State state) { m_state = state; }
    State state() const { return m_state; }

    void set_ready();
    void set_waiting(WaitingStatus&);
    bool block(Blocker&);
    void unblock_if_ready();

    void reap();

    void sys_exit(int status);
    pid_t sys_fork(TaskRegisters&);
    pid_t sys_waitpid(pid_t, int* wstatus, int options);
    int sys_execve(const char* pathname, char* const* argv);
    int sys_ioctl(int fd, uint32_t request, uint32_t* argp);
    int sys_open(const char*, int, mode_t);
    ssize_t sys_write(int fd, const void* buf, size_t count);
    ssize_t sys_read(int fd, void* buf, size_t count);
    int sys_fstat(int fd, struct stat* statbuf);
    pid_t sys_getpid();
    pid_t sys_getppid();
    uid_t sys_getuid();
    int sys_isatty(int fd);
    void* sys_mmap(const mmap_args*);
    int sys_munmap(void* addr, size_t length);
    int sys_dbgwrite(const char*, size_t);

private:
    static constexpr size_t kKernelStackSize = 16 * KB;
    static constexpr size_t kUserStackSize = 16 * KB;
    static constexpr size_t kMaxFileDescriptors = 64;

    Process(StringView name, pid_t pid, pid_t ppid, bool is_kernel, TTYDevice* = nullptr);
    Process(const Process& parent);

    ResultReturn<u32> load_elf();

    Result initialize_kernel_stack(const TaskRegisters&);
    ResultReturn<u32> initialize_user_stack(ArrayList<StringView>&& argv);

    bool is_address_accessible(VirtualAddress, size_t);
    int next_file_descriptor();
    ResultReturn<SharedPtr<FileDescriptor>> find_file_descriptor(int fd);

    void die();

    u8 m_ticks_left { 0 };

    String m_name;
    pid_t m_pid { 0 };
    pid_t m_ppid { 0 };
    User m_user;

    bool m_is_kernel { false };

    SharedPtr<TTYDevice> m_tty;
    SharedPtr<PageDirectory> m_page_directory;
    Blocker* m_blocker { nullptr };

    // TODO: Make sure this memory is being freed!
    ArrayList<VirtualRegion*> m_regions;
    Array<SharedPtr<FileDescriptor>, kMaxFileDescriptors> m_fds;

    UniquePtr<VirtualRegion> m_kernel_stack { nullptr };
    UniquePtr<VirtualRegion> m_user_stack { nullptr };
    u32* m_previous_stack_pointer { nullptr };

    State m_state;

    Process* m_next { nullptr };
    Process* m_previous { nullptr };
};
