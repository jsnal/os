/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Assert.h>
#include <Kernel/CPU/CPU.h>
#include <Kernel/DebugConsole.h>
#include <Kernel/Filesystem/VFS.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/POSIX.h>
#include <Kernel/Process/ELF.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Process/ProcessManager.h>
#include <LibC/errno_defines.h>
#include <Universal/Logger.h>
#include <Universal/Number.h>
#include <Universal/Stdlib.h>

#define DEBUG_PROCESS 0

Process::Process(StringView name, pid_t pid, pid_t ppid, bool is_kernel, TTYDevice* tty)
    : m_name(name)
    , m_pid(pid)
    , m_ppid(ppid)
    , m_user(User::root())
    , m_is_kernel(is_kernel)
    , m_tty(tty)
    , m_state(State::Runnable)
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
        m_fds[0] = tty->open(O_RDONLY).release_value();
        m_fds[1] = tty->open(O_WRONLY).release_value();
        m_fds[2] = tty->open(O_WRONLY).release_value();
    }
}

Process::Process(const Process& parent)
    : m_name(parent.name())
    , m_pid(PM.get_next_pid())
    , m_ppid(parent.pid())
    , m_user(User::root())
    , m_is_kernel(parent.is_kernel())
    , m_tty(parent.m_tty)
    , m_state(State::Runnable)
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
        m_fds[0] = *parent.m_fds[i];
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

ResultReturn<Process*> Process::create_kernel_process(StringView name, void (*entry_point)(), bool add_to_process_list)
{
    auto process = new Process(move(name), 0, 0, true);

    TaskRegisters regs = {};
    regs.frame.ss = 0;
    regs.frame.user_esp = 0;
    regs.frame.eflags = 0x0202;
    regs.frame.cs = CPU::SegmentSelector(CPU::Ring0, 1);
    regs.frame.eip = (u32)entry_point;
    regs.segment.ds = CPU::SegmentSelector(CPU::Ring0, 2);
    regs.segment.es = CPU::SegmentSelector(CPU::Ring0, 2);
    regs.segment.fs = CPU::SegmentSelector(CPU::Ring0, 2);
    regs.segment.gs = CPU::SegmentSelector(CPU::Ring0, 2);

    ENSURE(process->initialize_kernel_stack(regs));

    if (add_to_process_list) {
        PM.add_process(*process);
    }

    dbgprintf("Process", "Kernel Process '%s' spawned at 0x%x\n", process->m_name.data(), entry_point);
    return process;
}

ResultReturn<Process*> Process::create_user_process(StringView path, pid_t pid, pid_t ppid, ArrayList<StringView>&& argv, TTYDevice* tty)
{
    if (pid == 0) {
        pid = PM.get_next_pid();
    }
    auto process = new Process(path, pid, ppid, false, tty);

    argv.add_first(path);

    u32 entry_point = ENSURE_TAKE(process->load_elf());
    u32 user_esp = ENSURE_TAKE(process->initialize_user_stack(move(argv)));

    TaskRegisters regs = {};
    regs.frame.ss = CPU::SegmentSelector(CPU::Ring3, 4);
    regs.frame.user_esp = user_esp;
    regs.frame.eflags = 0x0202;
    regs.frame.cs = CPU::SegmentSelector(CPU::Ring3, 3);
    regs.frame.eip = (u32)entry_point;
    regs.segment.ds = CPU::SegmentSelector(CPU::Ring3, 4);
    regs.segment.es = CPU::SegmentSelector(CPU::Ring3, 4);
    regs.segment.fs = CPU::SegmentSelector(CPU::Ring3, 4);
    regs.segment.gs = CPU::SegmentSelector(CPU::Ring3, 4);
    ENSURE(process->initialize_kernel_stack(regs));

    PM.add_process(*process);

    dbgprintf("Process", "User Process '%s' (%u) spawned\n", process->m_name.data(), process->m_pid);
    return process;
}

ResultReturn<Process*> Process::fork_user_process(Process& parent, TaskRegisters& regs)
{
    auto child = new Process(parent);

    regs.general_purpose.eax = 0;
    ENSURE(child->initialize_kernel_stack(regs));

    for (size_t i = 0; i < parent.m_regions.size(); i++) {
        ENSURE_TAKE(child->allocate_region_at(parent.m_regions[i]->lower(), parent.m_regions[i]->length(), parent.m_regions[i]->access()));
        parent.m_regions[i]->copy(*child->m_regions[i]);
    }

    // For now the user stack will always be the first region
    child->m_user_stack = child->m_regions[0];

    PM.add_process(*child);

    dbgprintf("Process", "User Process '%s' (%u) forked to spawn %u\n", parent.m_name.data(), parent.m_pid, child->m_pid);
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

    dbgprintf_if(DEBUG_PROCESS, "Process", "Allocated virtual region 0x%x - 0x%x for Process '%s'\n", m_regions.last()->lower(), m_regions.last()->upper(), name().data());

    return m_regions.last();
}

Result Process::deallocate_region(size_t index)
{
    if (index < 0 || index > m_regions.size()) {
        return Result::Failure;
    }

    dbgprintf_if(DEBUG_PROCESS, "Process", "Deallocating virtual region 0x%x - 0x%x for Process '%s'\n", m_regions[index]->lower(), m_regions[index]->upper(), name().data());

    ENSURE(m_regions[index]->unmap(page_directory()));
    ENSURE(page_directory().address_allocator().free(m_regions[index]->address_range()));
    ENSURE(m_regions[index]->free());
    return Result::OK;
}

void Process::context_switch(Process* next_process)
{
    m_ticks_left = 0;
    next_process->reset_timer_ticks();
    PM.tss()->esp0 = next_process->m_kernel_stack->upper();
    ::context_switch(&m_previous_stack_pointer, next_process->m_previous_stack_pointer, next_process->cr3());
}

Result Process::initialize_kernel_stack(const TaskRegisters& regs)
{
    m_kernel_stack = MM.allocate_kernel_region(kKernelStackSize);
    const u32 capacity = kKernelStackSize / sizeof(u32);

    if (m_kernel_stack.ptr() == nullptr) {
        return Result::Failure;
    }

#if DEBUG_PROCESS
    dbgprintf("Process", "Initializing Kernel stack:\n");
    dbgprintf("Process", "  EAX=%#x EBX=%#x ECX=%#x EDX=%#x\n", regs.general_purpose.eax, regs.general_purpose.ebx, regs.general_purpose.ecx, regs.general_purpose.edx);
    dbgprintf("Process", "  ESI=%#x EDI=%#x EBP=%#x ESP=%#x\n", regs.general_purpose.esi, regs.general_purpose.edi, regs.general_purpose.ebp, regs.general_purpose.esp);
    dbgprintf("Process", "  GS=%#x FS=%#x ES=%#x DS=%#x\n", regs.segment.gs, regs.segment.fs, regs.segment.es, regs.segment.ds);
    dbgprintf("Process", "  EIP=%#x CS=%#x EFLAGS=%#x\n", regs.frame.eip, regs.frame.cs, regs.frame.eflags);
#endif

    u32* stack = reinterpret_cast<u32*>(m_kernel_stack->lower().get());
    stack[capacity - 1] = regs.frame.ss;              // SS
    stack[capacity - 2] = regs.frame.user_esp;        // ESP
    stack[capacity - 3] = regs.frame.eflags;          // EFLAGS
    stack[capacity - 4] = regs.frame.cs;              // CS
    stack[capacity - 5] = regs.frame.eip;             // EIP
    stack[capacity - 6] = regs.general_purpose.eax;   // EAX
    stack[capacity - 7] = regs.general_purpose.ebx;   // EBX
    stack[capacity - 8] = regs.general_purpose.ecx;   // ECX
    stack[capacity - 9] = regs.general_purpose.edx;   // EDX
    stack[capacity - 10] = regs.general_purpose.ebp;  // EBP
    stack[capacity - 11] = regs.general_purpose.edi;  // EDI
    stack[capacity - 12] = regs.general_purpose.esi;  // ESI
    stack[capacity - 13] = regs.segment.ds;           // DS
    stack[capacity - 14] = regs.segment.es;           // ES
    stack[capacity - 15] = regs.segment.fs;           // FS
    stack[capacity - 16] = regs.segment.gs;           // GS
    stack[capacity - 17] = (u32)first_context_switch; // Entry point
    stack[capacity - 18] = 0x0202;                    // EFLAGS, always start the same
    stack[capacity - 19] = regs.general_purpose.ebx;  // EBX
    stack[capacity - 20] = regs.general_purpose.esi;  // EDI
    stack[capacity - 21] = regs.general_purpose.edi;  // EDI
    stack[capacity - 22] = 0;                         // EBP

    m_previous_stack_pointer = stack + (capacity - 22);

    return Result::OK;
}

ResultReturn<u32> Process::initialize_user_stack(ArrayList<StringView>&& argv)
{
    m_user_stack = ENSURE_TAKE(allocate_region(kUserStackSize, VirtualRegion::Read | VirtualRegion::Write));
    auto temporary_mapping = ENSURE_TAKE(MM.temporary_map(m_user_stack->physical_pages()[m_user_stack->physical_pages().size() - 1]));

    const u32 capacity = Memory::kPageSize / sizeof(u32);
    u32* stack = reinterpret_cast<u32*>(temporary_mapping.ptr());
    stack[capacity - 1] = 0xDEAD0000;

    auto temporary_address_to_user_address = [&](u32 address) {
        u32 user_stack_top = m_user_stack->lower().get() + kUserStackSize;
        u32 temporary_stack_top = reinterpret_cast<u32>(stack + capacity);
        return user_stack_top - (temporary_stack_top - address);
    };

    // Stack layout:
    //   0xDEAD0000
    //   argv[N..0]
    //   strings[0..N]
    //   argv**
    //   argc

    char** stack_argv = reinterpret_cast<char**>(stack + capacity - argv.size() - 1);
    char* stack_argv_buffer = reinterpret_cast<char*>(stack_argv - 1);

    for (size_t i = 0; i < argv.size(); i++) {
        char* buffer_start = stack_argv_buffer - argv[i].length();

        // Copy the argument string to the stack
        memcpy(buffer_start, argv[i].str(), argv[i].length());
        *stack_argv_buffer = '\0';
        stack_argv_buffer = buffer_start - 1;

        // Set the pointer to the string
        stack_argv[i] = (char*)temporary_address_to_user_address((u32)buffer_start);
    }

    // Round to the nearest u32 border to add the arguments that will eventually goto main()
    u32* stack_after_args = reinterpret_cast<u32*>(reinterpret_cast<u32>(stack_argv_buffer) & ~0x3) - 1;
    *(stack_after_args--) = 0;
    *(stack_after_args--) = temporary_address_to_user_address((u32)stack_argv);
    *(stack_after_args--) = argv.size();

    MM.temporary_unmap();
    return temporary_address_to_user_address(reinterpret_cast<u32>(stack_after_args));
}

ResultReturn<u32> Process::load_elf()
{
    auto fd = ENSURE_TAKE(VFS::the().open(m_name.data(), 0, 0));
    auto elf_result = ENSURE_TAKE(ELF::create(fd));
    auto elf_program_headers = ENSURE_TAKE(elf_result->read_program_headers());

    for (size_t i = 0; i < elf_program_headers.size(); i++) {
        auto program_header = elf_program_headers[i];
        if (program_header.p_type == PT_LOAD) {
            size_t load_location = Memory::page_round_down(program_header.p_vaddr);
            size_t load_memory_size = Memory::page_round_up(program_header.p_memsz);

            auto region = ENSURE_TAKE(allocate_region_at(load_location, load_memory_size, ELF::program_flags_to_access(program_header.p_flags)));

            // TODO: Find a faster way to do this
            auto tmp_kernel_region = MM.allocate_kernel_region_at(region->physical_pages().first(), load_memory_size);
            fd->seek(program_header.p_offset, SEEK_SET);
            fd->read(tmp_kernel_region->lower().ptr(), program_header.p_filesz);

            MM.free_kernel_region(*tmp_kernel_region);
        }
    }

    return elf_result->header().e_entry;
}

void Process::set_ready()
{
    dbgprintf_if(DEBUG_PROCESS, "Process", "Setting '%s' to Ready\n", name().data());
    m_state = Runnable;
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

bool Process::block(Blocker& blocker)
{
    if (m_state != Running || m_blocker != nullptr) {
        return false;
    }

    if (blocker.is_ready()) {
        return true;
    }

    m_blocker = &blocker;
    m_state = Blocked;

    PM.yield();

    m_blocker = nullptr;
    return true;
}

void Process::unblock_if_ready()
{
    ASSERT(m_state == Blocked && m_blocker != nullptr);
    if (m_blocker->is_ready()) {
        m_state = Runnable;
    }
}

void Process::reap()
{
    m_pid = 0;
    m_ppid = 0;
    MM.free_kernel_region(*m_kernel_stack);
    PM.remove_process(*this);
}

bool Process::is_address_accessible(VirtualAddress address, size_t length)
{
    for (int i = 0; i < m_regions.size(); i++) {
        if (m_regions[i]->is_accessible(address, length)) {
            return true;
        }
    }
    return false;
}

int Process::next_file_descriptor()
{
    int fd = -EMFILE;
    for (size_t i = 0; i < kMaxFileDescriptors; i++) {
        if (m_fds[i].ptr() == nullptr) {
            fd = i;
            break;
        }
    }

    return fd;
}

ResultReturn<SharedPtr<FileDescriptor>> Process::find_file_descriptor(int fd)
{
    if (fd < 0 || fd > m_fds.size() || m_fds[fd].is_null()) {
        return Result(Result::Failure);
    }
    return m_fds[fd];
}

void Process::die()
{
    for (int i = 0; i < m_regions.size(); i++) {
        deallocate_region(i);
    }

    m_state = Process::Dead;
}

void Process::sys_exit(int status)
{
    dbgprintf("Process", "'%s' (%u) exited with status %d\n", m_name.data(), m_pid, status);
    die();
}

pid_t Process::sys_fork(TaskRegisters& regs)
{
    auto fork_result = Process::fork_user_process(*this, regs);
    if (fork_result.is_error()) {
        return -EFAULT;
    }

    auto child = fork_result.release_value();
    return child->pid();
}

pid_t Process::sys_waitpid(pid_t pid, int* wstatus, int options)
{
    WaitBlocker blocker(*this, pid, options);
    if (!block(blocker)) {
        return -ECHILD;
    }

    PM.enter_critical();

    auto* p = PM.from_pid(pid);
    if (p == nullptr) {
        return -ECHILD;
    }

    if (p->is_dead()) {
        p->reap();
        delete p;
    }

    PM.exit_critical();
    return pid;
}

int Process::sys_execve(const char* pathname, char* const* argv)
{
    {
        ArrayList<StringView> arguments;
        for (size_t i = 0; argv[i]; ++i) {
            arguments.add_last(argv[i]);
        }

        auto execve_result = Process::create_user_process(pathname, m_pid, m_ppid, move(arguments), m_tty.ptr());
        if (execve_result.is_error()) {
            return -EFAULT;
        }

        auto new_process = execve_result.release_value();
    }

    die();
    PM.yield();
    return -1;
}

int Process::sys_ioctl(int fd, uint32_t request, uint32_t* argp)
{
    if (!is_address_accessible(reinterpret_cast<u32>(argp), sizeof(uint32_t))) {
        return -EFAULT;
    }

    auto fd_result = find_file_descriptor(fd);
    if (fd_result.is_error()) {
        return -EBADF;
    }
    return fd_result.release_value()->ioctl(request, argp);
}

int Process::sys_open(const char* pathname, int flags, mode_t mode)
{
    int fd = next_file_descriptor();
    dbgprintln("Process", "Next fd = %d", fd);

    auto result = VFS::the().open(pathname, flags, mode);
    if (result.is_error()) {
        // dbgprintln("Process", "ERROR = %d", result.error());

        return result.error();
    }

    m_fds[fd] = result.release_value();
    return fd;
}

ssize_t Process::sys_write(int fd, const void* buf, size_t count)
{
    if (count < 0) {
        return -EINVAL;
    }

    if (count == 0) {
        return 0;
    }

    if (!is_address_accessible((u32)buf, count)) {
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

    if (!is_address_accessible((u32)buf, count)) {
        return -EFAULT;
    }

    auto fd_result = find_file_descriptor(fd);
    if (fd_result.is_error()) {
        return -EBADF;
    }

    return fd_result.release_value()->read((u8*)buf, count);
}

pid_t Process::sys_getpid()
{
    return m_pid;
}

pid_t Process::sys_getppid()
{
    return m_ppid;
}

uid_t Process::sys_getuid()
{
    dbgprintf("Process", "'%s' (%u) called getuid() %d\n", m_name.data(), m_pid, m_user.uid());
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

void* Process::sys_mmap(const mmap_args* args)
{
    if (!is_address_accessible(reinterpret_cast<u32>(args), sizeof(mmap_args))) {
        return (void*)-EFAULT;
    }

    auto& [addr, length, prot, flags, fd, offset] = *args;

    if ((flags & MAP_SHARED && flags & MAP_PRIVATE) || (!(flags & MAP_SHARED) && !(flags & MAP_PRIVATE)) || length == 0) {
        return (void*)-EINVAL;
    }

    if (flags & MAP_FIXED && (!Memory::is_page_aligned(reinterpret_cast<u32>(addr)) || !Memory::is_page_aligned(offset))) {
        return (void*)-EINVAL;
    }

    if (flags & MAP_FIXED && !is_address_accessible(reinterpret_cast<u32>(addr), length)) {
        return (void*)-ENOMEM;
    }

    auto allocate_result = allocate_region(length, prot);
    if (allocate_result.is_error()) {
        return (void*)-ENOMEM;
    }

    return allocate_result.value()->lower().ptr();
}

int Process::sys_munmap(void* addr, size_t length)
{
    VirtualAddress unmap_address(reinterpret_cast<u32>(addr));
    if (!is_address_accessible(unmap_address, length) || length == 0 || !Memory::is_page_aligned(unmap_address)) {
        return -EINVAL;
    }

    size_t i = 0;
    for (; i < m_regions.size(); i++) {
        if (m_regions[i]->contains(unmap_address)) {
            break;
        }
    }

    if (i >= m_regions.size()) {
        return -EINVAL;
    }

    VirtualAddress unmap_lower = Memory::page_round_down(unmap_address);
    VirtualAddress unmap_upper = Memory::page_round_up(unmap_address.offset(length));
    size_t unmap_page_count = ceiling_divide((unmap_address.offset(length)) - unmap_address, Memory::kPageSize);

    VirtualAddress old_lower_address = m_regions[i]->lower();
    VirtualAddress old_upper_address = m_regions[i]->upper();
    size_t old_page_count = m_regions[i]->page_count();
    u8 old_access = m_regions[i]->access();

    ASSERT(deallocate_region(i).is_ok());

    // TODO: Probably should do this in deallocate_region but causes issues when freeing in loops
    ASSERT(m_regions.remove(i).is_ok());

    // All pages are being freed so no need to reallocate
    if (old_page_count == unmap_page_count) {
        return 0;
    }

    size_t length_before_address = unmap_lower - old_lower_address;
    size_t length_after_address = old_upper_address - unmap_upper;
    if (length_after_address > 0) {
        ASSERT(allocate_region_at(unmap_upper, length_after_address, old_access).is_ok());
    }
    if (length_before_address > 0) {
        ASSERT(allocate_region_at(old_lower_address, length_before_address, old_access).is_ok());
    }
    return 0;
}

int Process::sys_dbgwrite(const char* buf, size_t length)
{
    DebugConsole::the().write(buf, length);
    return 0;
}

void Process::dump_stack(bool kernel) const
{
    dbgprintf("Process", "%s Stack\n", kernel ? "Kernel" : "User");
    u32 stack = kernel ? (u32)m_kernel_stack->upper() : (u32)m_user_stack->upper();
    for (int i = 15 * sizeof(u32); i >= 0; i -= sizeof(u32)) {
        dbgprintf("Process", "%x:%x\n", (stack - i), *(u32*)(stack - i));
    }
}
