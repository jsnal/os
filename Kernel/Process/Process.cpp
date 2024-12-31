/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Assert.h>
#include <Kernel/CPU/CPU.h>
#include <Kernel/Filesystem/VFS.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/POSIX.h>
#include <Kernel/Process/ELF.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Process/ProcessManager.h>
#include <LibC/errno_defines.h>
#include <Universal/Stdlib.h>

Process::Process(const String& name, pid_t pid, uid_t uid, gid_t gid, bool is_kernel, TTYDevice* tty)
    : m_name(move(name))
    , m_pid(pid)
    , m_user(User::root())
    , m_is_kernel(is_kernel)
    , m_tty(tty)
    , m_state(State::Created)
{
    if (is_kernel) {
        m_page_directory = MM.kernel_page_directory();
    } else {
        m_page_directory = PageDirectory::create_user_page_directory();
        m_page_directory->set_base(MM.allocate_physical_kernel_page());

        // Copy Kernel identity map and higher-half
        m_page_directory->entries()[0].copy(MM.kernel_page_directory().entries()[0]);
        for (u32 page = 768; page < 1024; page++) {
            m_page_directory->entries()[page].copy(MM.kernel_page_directory().entries()[page]);
        }
    }

    reset_timer_ticks();

    if (tty != nullptr) {
        m_fds.add(0, tty->open(O_RDONLY).value());
        m_fds.add(1, tty->open(O_WRONLY).value());
        m_fds.add(2, tty->open(O_WRONLY).value());
    }
}

Process::Process(const Process& parent)
    : m_name(parent.name())
    , m_pid(PM.get_next_pid())
    , m_user(User::root())
    , m_is_kernel(parent.is_kernel())
    , m_tty(parent.m_tty)
    , m_state(State::Created)
{
    if (parent.is_kernel()) {
        panic("Kernel processes may not be forked\n");
    }

    m_page_directory = PageDirectory::create_user_page_directory();
    m_page_directory->set_base(MM.allocate_physical_kernel_page());

    // Copy Kernel identity map and higher-half
    m_page_directory->entries()[0].copy(MM.kernel_page_directory().entries()[0]);
    for (u32 page = 768; page < 1024; page++) {
        m_page_directory->entries()[page].copy(MM.kernel_page_directory().entries()[page]);
    }

    for (size_t i = 0; i < parent.m_fds.size(); i++) {
        if (parent.m_fds[i].ptr() == nullptr) {
            continue;
        }
        m_fds.add(i, parent.m_fds[i]);
    }
}

Process::~Process()
{
    // TODO: Manually deleting this right now because storing UniquePtr in
    // ArrayList does not currently work
    for (size_t i = 0; i < m_regions.size(); i++) {
        delete m_regions[i];
    }
}

ResultReturn<Process*> Process::create_kernel_process(const String& name, void (*entry_point)(), bool add_to_process_list)
{
    pid_t pid = add_to_process_list ? PM.get_next_pid() : 0;
    auto process = new Process(move(name), pid, 0, 0, true);
    process->m_entry_point = entry_point;

    SyscallRegisters regs = {};
    regs.frame.eip = reinterpret_cast<u32>(process->m_entry_point);
    regs.frame.eflags = 0x0202;

    ENSURE(process->initialize_kernel_stack(regs));

    if (add_to_process_list) {
        PM.add_process(*process);
    }

    dbgprintf("Process", "Kernel Process '%s' spawned at 0x%x\n", process->m_name.str(), entry_point);
    return process;
}

Result Process::create_user_process(const String& path, uid_t uid, gid_t gid, TTYDevice* tty)
{
    auto process = new Process(move(path), PM.get_next_pid(), uid, gid, false, tty);

    SyscallRegisters regs = {};
    regs.frame.eip = reinterpret_cast<u32>(prepare_user_process);
    regs.frame.eflags = 0x0202;

    ENSURE(process->initialize_kernel_stack(regs));
    ENSURE(process->initialize_user_stack());

    PM.add_process(*process);

    dbgprintf("Process", "User Process '%s' (%u) spawned\n", process->m_name.str(), process->m_pid);
    return Result::OK;
}

ResultReturn<Process*> Process::fork_user_process(Process& parent, SyscallRegisters& regs)
{
    auto child = new Process(parent);

    child->m_user_stack = parent.m_user_stack->clone();
    regs.general_purpose.eax = 0;
    // regs.frame.eip = 0xDEADBEEF;

    ENSURE(child->initialize_kernel_stack(regs));
    ENSURE(child->initialize_user_stack());

    // child->m_user_stack->map(*parent.m_page_directory);
    // memcpy(child->m_user_stack.ptr(), parent.m_user_stack.ptr(), parent.m_user_stack->address_range().length());

    for (size_t i = 0; i < parent.m_regions.size(); i++) {
        dbgprintf("Process", "Parent Region %u: %#08x - %#08x (%#08x)\n", i, parent.m_regions[i]->lower(),
            parent.m_regions[i]->upper(), parent.m_regions[i]->physical_pages()[i]);

        child->m_regions.add_last(parent.m_regions[i]->clone().leak_ptr());
        child->m_regions.last()->map(*child->m_page_directory);

        dbgprintf("Process", "Child Region %u: %#08x - %#08x (%#08x)\n", i, child->m_regions[i]->lower(),
            child->m_regions[i]->upper(), child->m_regions[i]->physical_pages()[i]);
    }

    PM.add_process(*child);

    dbgprintf("Process", "User Process '%s' (%u) forked to spawn %u\n", parent.m_name.str(), parent.m_pid, child->m_pid);
    return child;
}

ResultReturn<VirtualRegion*> Process::allocate_region(size_t size, u8 access)
{
    return allocate_region_at(VirtualAddress(), size, access);
}

ResultReturn<VirtualRegion*> Process::allocate_region_at(VirtualAddress virtual_address, size_t size, u8 access)
{
    AddressRange range;
    if (virtual_address.is_null()) {
        range = ENSURE_TAKE(page_directory().address_allocator().allocate(size));
    } else {
        range = ENSURE_TAKE(page_directory().address_allocator().allocate_at(virtual_address, size));
    }

    m_regions.add_last(VirtualRegion::create_user_region(range, access).leak_ptr());
    m_regions.last()->map(page_directory());

    dbgprintf_if(DEBUG_PROCESS, "Process", "Allocated virtual region 0x%x - 0x%x for Process '%s'\n", m_regions.last()->lower(), m_regions.last()->upper(), name().str());

    return m_regions.last();
}

Result Process::deallocate_region(size_t index)
{
    if (index < 0 || index > m_regions.size()) {
        return Result::Failure;
    }

    dbgprintf_if(DEBUG_PROCESS, "Process", "Deallocating virtual region 0x%x - 0x%x for Process '%s'\n", m_regions[index]->lower(), m_regions[index]->upper(), name().str());

    m_regions[index]->unmap(page_directory());
    m_regions[index]->free();
    return Result::OK;
}

void Process::context_switch(Process* next_process)
{
    m_ticks_left = 0;
    next_process->reset_timer_ticks();
    PM.tss()->esp0 = next_process->m_kernel_stack->upper();
    do_context_switch(&m_previous_stack_pointer, next_process->m_previous_stack_pointer, next_process->cr3());
}

Result Process::initialize_kernel_stack(const SyscallRegisters& regs)
{
    m_kernel_stack = MM.allocate_kernel_region(kKernelStackSize);
    const u32 capacity = kKernelStackSize / sizeof(u32);

    if (m_kernel_stack.ptr() == nullptr) {
        return Result::Failure;
    }

    dbgprintf("Process", "EAX=%x EBX=%x ECX=%x EDX=%x\n", regs.general_purpose.eax, regs.general_purpose.ebx, regs.general_purpose.ecx, regs.general_purpose.edx);
    dbgprintf("Process", "ESI=%x EDI=%x EBP=%x ESP=%x\n", regs.general_purpose.esi, regs.general_purpose.edi, regs.general_purpose.ebp, regs.general_purpose.esp);
    dbgprintf("Process", "DS=%x CS=%x SS=%x\n", regs.segment.ds, regs.frame.cs, regs.frame.ss);
    dbgprintf("Process", "EFLAGS=%x\n", regs.frame.eflags);
    dbgprintf("Process", "EIP = %x\n", regs.frame.eip);

    u32* stack = reinterpret_cast<u32*>(m_kernel_stack->lower().get());

    stack[capacity - 1] = 0x0000DEAD;                          // Fallback return address
    stack[capacity - 2] = CPU::SegmentSelector(CPU::Ring3, 4); // User mode SS
    stack[capacity - 3] = 0;                                   // User mode ESP, setup later
    stack[capacity - 4] = regs.frame.eflags;                   // User mode EFLAGS
    stack[capacity - 5] = CPU::SegmentSelector(CPU::Ring3, 3); // CS for user mode
    stack[capacity - 6] = 0;                                   // User mode EIP, setup later
    stack[capacity - 7] = 0xDEADBEEF;                          // User mode entry point, bad address as a placeholder
    stack[capacity - 8] = regs.frame.eip;                      // EIP
    stack[capacity - 9] = regs.general_purpose.eax;            // EAX
    stack[capacity - 10] = regs.general_purpose.ecx;           // ECX
    stack[capacity - 11] = regs.general_purpose.edx;           // EDX
    stack[capacity - 12] = regs.general_purpose.ebx;           // EBX
    stack[capacity - 13] = regs.general_purpose.esp;           // ESP
    stack[capacity - 14] = regs.general_purpose.ebp;           // EBP
    stack[capacity - 15] = regs.general_purpose.esi;           // ESI
    stack[capacity - 16] = regs.general_purpose.edi;           // EDI
    stack[capacity - 17] = regs.frame.eflags;                  // EFLAGS
                                                               //    stack[capacity - 18] = CPU::SegmentSelector(CPU::Ring0, 2); // CS
                                                               //    stack[capacity - 19] = CPU::SegmentSelector(CPU::Ring0, 2); // FS
                                                               //    stack[capacity - 20] = CPU::SegmentSelector(CPU::Ring0, 2); // ES
                                                               //    stack[capacity - 21] = CPU::SegmentSelector(CPU::Ring0, 2); // DS
    stack[capacity - 18] = regs.segment.gs;                    // CPU::SegmentSelector(CPU::Ring0, 2); // GS
    stack[capacity - 19] = regs.segment.fs;                    // CPU::SegmentSelector(CPU::Ring0, 2); // FS
    stack[capacity - 20] = regs.segment.es;                    // CPU::SegmentSelector(CPU::Ring0, 2); // ES
    stack[capacity - 21] = regs.segment.ds;                    // CPU::SegmentSelector(CPU::Ring0, 2); // DS

    m_previous_stack_pointer = stack + (capacity - 21);

    return Result::OK;
}

Result Process::initialize_user_stack()
{
    m_user_stack = ENSURE_TAKE(allocate_region(kUserStackSize, VirtualRegion::Read | VirtualRegion::Write));
    return Result::OK;
}

void Process::prepare_user_process()
{
    auto& process = PM.current_process();

    constexpr u32 kernel_stack_capacity = kKernelStackSize / sizeof(u32);
    constexpr u32 user_stack_capacity = kUserStackSize / sizeof(u32);

    u32* user_stack = reinterpret_cast<u32*>(process.m_user_stack->lower().ptr());
    u32* kernel_stack = reinterpret_cast<u32*>(process.m_kernel_stack->lower().ptr());

    ASSERT(process.load_elf().is_ok());

    // Setup the kernel stack to switch to user mode when this function returns. The start_user_process()
    // function simply calls 'iret' which sets up the rest of the registers. See initialize_kernel_stack()
    // for the stack layout at very beginning of a process.
    kernel_stack[kernel_stack_capacity - 3] = reinterpret_cast<u32>(user_stack + (user_stack_capacity - 4));
    kernel_stack[kernel_stack_capacity - 6] = reinterpret_cast<u32>(process.m_entry_point);
    kernel_stack[kernel_stack_capacity - 7] = reinterpret_cast<u32>(start_user_process);

    // Setup the user process stack
    user_stack[user_stack_capacity - 1] = 0xDEAD0000; // Fallback return address
    user_stack[user_stack_capacity - 2] = 0;
    user_stack[user_stack_capacity - 3] = 0;
    user_stack[user_stack_capacity - 4] = 0;

    CPU::set_ds_register(CPU::SegmentSelector(CPU::Ring3, 4));
    CPU::set_es_register(CPU::SegmentSelector(CPU::Ring3, 4));
    CPU::set_fs_register(CPU::SegmentSelector(CPU::Ring3, 4));
    CPU::set_gs_register(CPU::SegmentSelector(CPU::Ring3, 4));
}

Result Process::load_elf()
{
    auto fd = ENSURE_TAKE(VFS::the().open(m_name, 0, 0));
    auto elf_result = ENSURE_TAKE(ELF::create(fd));
    auto elf_program_headers = ENSURE_TAKE(elf_result->read_program_headers());

    for (size_t i = 0; i < elf_program_headers.size(); i++) {
        auto program_header = elf_program_headers[i];
        if (program_header.p_type == PT_LOAD) {
            size_t load_location = Memory::page_round_down(program_header.p_vaddr);
            size_t load_memory_size = Memory::page_round_up(program_header.p_memsz);

            auto region = ENSURE_TAKE(allocate_region_at(load_location, load_memory_size, ELF::program_flags_to_access(program_header.p_flags)));
            fd->seek(program_header.p_offset, SEEK_SET);
            fd->read(region->lower().ptr(), program_header.p_filesz);
        }
    }

    m_entry_point = (void (*)())elf_result->header().e_entry;
    return Result::OK;
}

void Process::set_ready()
{
    dbgprintf_if(DEBUG_PROCESS, "Process", "Setting '%s' to Ready\n", name().str());
    m_state = Ready;
}

void Process::set_waiting(WaitingStatus& waiting_status)
{
    PM.enter_critical();

    if (waiting_status.is_ready()) {
        set_ready();
        PM.exit_critical();
        return;
    }

    m_state = Waiting;
    PM.exit_critical();
    PM.yield();
}

void Process::reap()
{
    MM.free_kernel_region(*m_kernel_stack);
    PM.remove_process(*this);
}

bool Process::is_address_accessible(VirtualAddress address)
{
    for (int i = 0; i < m_regions.size(); i++) {
        if (m_regions[i]->contains(address)) {
            return true;
        }
    }
    return false;
}

ResultReturn<SharedPtr<FileDescriptor>> Process::find_file_descriptor(int fd)
{
    if (fd < 0 || fd > m_fds.size() || m_fds[fd].is_null()) {
        return Result(Result::Failure);
    }
    return m_fds[fd];
}

ssize_t Process::sys_write(int fd, const void* buf, size_t count)
{
    if (count < 0) {
        return -EINVAL;
    }

    if (count == 0) {
        return 0;
    }

    if (!is_address_accessible((u32)buf)) {
        return -EFAULT;
    }

    auto fd_result = find_file_descriptor(fd);
    if (fd_result.is_error()) {
        return -EBADF;
    }

    return fd_result.release_value()->write((const u8*)buf, count);
}

ssize_t Process::sys_read(int fd, void* buf, size_t count)
{
    if (count < 0) {
        return -EINVAL;
    }

    if (count == 0) {
        return 0;
    }

    if (!is_address_accessible((u32)buf)) {
        return -EFAULT;
    }

    auto fd_result = find_file_descriptor(fd);
    if (fd_result.is_error()) {
        return -EBADF;
    }

    return fd_result.release_value()->read((u8*)buf, count);
}

void Process::sys_exit(int status)
{
    dbgprintf("Process", "'%s' (%u) exited with status %d\n", m_name.str(), m_pid, status);

    for (int i = 0; i < m_regions.size(); i++) {
        deallocate_region(i);
    }

    m_state = Process::Dead;
}

pid_t Process::sys_fork(SyscallRegisters& regs)
{
    auto fork_result = Process::fork_user_process(*this, regs);
    auto child = ENSURE_TAKE(fork_result);

    return child->pid();
}

int Process::sys_ioctl(int fd, uint32_t request, uint32_t* argp)
{
    if (!is_address_accessible((u32)argp)) {
        return -EFAULT;
    }

    auto fd_result = find_file_descriptor(fd);
    if (fd_result.is_error()) {
        return -EBADF;
    }
    return fd_result.release_value()->ioctl(request, argp);
}

pid_t Process::sys_getpid()
{
    return m_pid;
}

uid_t Process::sys_getuid()
{
    dbgprintf("Process", "'%s' (%u) called getuid() %d\n", m_name.str(), m_pid, m_user.uid());
    return m_user.uid();
}

int Process::sys_isatty(int fd)
{
    auto fd_result = find_file_descriptor(fd);
    if (fd_result.is_error()) {
        return -EBADF;
    }
    return fd_result.release_value()->file()->is_tty_device();
}

void Process::dump_stack(bool kernel) const
{
    dbgprintf("Process", "%s Stack\n", kernel ? "Kernel" : "User");
    u32 stack = kernel ? (u32)m_kernel_stack->upper() : (u32)m_user_stack->upper();
    for (int i = 15 * sizeof(u32); i >= 0; i -= sizeof(u32)) {
        dbgprintf("Process", "%x:%x\n", (stack - i), *(u32*)(stack - i));
    }
}
