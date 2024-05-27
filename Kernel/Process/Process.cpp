#include <Kernel/Assert.h>
#include <Kernel/CPU/CPU.h>
#include <Kernel/Filesystem/VFS.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Process/ELF.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Process/ProcessManager.h>
#include <Universal/Stdlib.h>

#define KERNEL_STACK_SIZE (16 * KB)
#define USER_STACK_SIZE (16 * KB)

Process::Process(const String& name, pid_t pid, uid_t uid, gid_t gid, bool is_kernel)
    : m_name(move(name))
    , m_pid(pid)
    , m_uid(uid)
    , m_gid(gid)
    , m_is_kernel(is_kernel)
    , m_state(State::Created)
{
    if (is_kernel) {
        m_page_directory = MM.kernel_page_directory();
    } else {
        m_page_directory = PageDirectory::create_user_page_directory(*this);
        m_page_directory->set_base(MM.allocate_physical_kernel_page());

        // Copy Kernel identity map and higher-half
        m_page_directory->entries()[0].copy(MM.kernel_page_directory().entries()[0]);
        for (u32 page = 768; page < 1024; page++) {
            m_page_directory->entries()[page].copy(MM.kernel_page_directory().entries()[page]);
        }
    }
}

ResultReturn<Process*> Process::create_kernel_process(const String& name, void (*entry_point)(), bool add_to_process_list)
{
    pid_t pid = add_to_process_list ? PM.get_next_pid() : 0;
    auto process = new Process(move(name), pid, 0, 0, true);

    auto result = process->initialize_kernel_stack();
    ENSURE(result);

    process->m_entry_point = entry_point;

    if (add_to_process_list) {
        PM.add_process(*process);
    }

    dbgprintf("Process", "Kernel Process '%s' (%u) spawned at 0x%x\n", process->m_name.str(), process->m_pid, entry_point);
    return process;
}

Result Process::create_user_process(const String& path, uid_t uid, gid_t gid)
{
    auto process = new Process(move(path), PM.get_next_pid(), uid, gid, false);

    auto stack_result = process->initialize_kernel_stack();
    ENSURE(stack_result);

    stack_result = process->initialize_user_stack();
    ENSURE(stack_result);

    PM.add_process(*process);

    dbgprintf("Process", "User Process '%s' (%u) spawned\n", process->m_name.str(), process->m_pid);
    return Result::OK;
}

ResultReturn<VirtualRegion*> Process::allocate_region(size_t size, u8 access)
{
    return allocate_region_at(VirtualAddress(), size, access);
}

ResultReturn<VirtualRegion*> Process::allocate_region_at(VirtualAddress virtual_address, size_t size, u8 access)
{
    ResultReturn<AddressRange> range_result;
    if (virtual_address.is_null()) {
        range_result = page_directory().address_allocator().allocate(size);
    } else {
        range_result = page_directory().address_allocator().allocate_at(virtual_address, size);
    }
    ENSURE(range_result);

    m_regions.add_last(VirtualRegion::create_user_region(range_result.value(), access).leak_ptr());
    m_regions.last()->map(page_directory());

    dbgprintf_if(DEBUG_PROCESS, "Process", "Allocated virtual region 0x%x - 0x%x for Process '%s'\n", m_regions.last()->lower(), m_regions.last()->upper(), name().str());
    dbgprintf("Process", "Allocated virtual region 0x%x - 0x%x for Process '%s'\n", m_regions.last()->lower(), m_regions.last()->upper(), name().str());

    return m_regions.last();
}

void Process::context_switch(Process* next_process)
{
    PM.tss()->esp0 = next_process->m_kernel_stack->upper();
    do_context_switch(&m_previous_stack_pointer, next_process->m_previous_stack_pointer, next_process->cr3());
}

Result Process::initialize_kernel_stack()
{
    m_kernel_stack = MM.allocate_kernel_region(KERNEL_STACK_SIZE);
    const u32 capacity = KERNEL_STACK_SIZE / sizeof(u32);

    if (m_kernel_stack.ptr() == nullptr) {
        return Result::Failure;
    }

    u32* stack = reinterpret_cast<u32*>(m_kernel_stack->lower().get());

    stack[capacity - 1] = 0x0000DEAD;                // Fallback return address
    stack[capacity - 2] = (u32)new_process_runnable; // EIP
    stack[capacity - 3] = 0;                         // EAX
    stack[capacity - 4] = 0;                         // ECX
    stack[capacity - 5] = 0;                         // EDX
    stack[capacity - 6] = 0;                         // EBX
    stack[capacity - 7] = 0;                         // ESP
    stack[capacity - 8] = 0;                         // EBP
    stack[capacity - 9] = 0;                         // ESI
    stack[capacity - 10] = 0;                        // EDI

    stack[capacity - 11] = 0x0202; // EFLAGS

    stack[capacity - 12] = CPU::SegmentSelector(CPU::Ring0, 2); // GS
    stack[capacity - 13] = CPU::SegmentSelector(CPU::Ring0, 2); // FS
    stack[capacity - 14] = CPU::SegmentSelector(CPU::Ring0, 2); // ES
    stack[capacity - 15] = CPU::SegmentSelector(CPU::Ring0, 2); // DS

    m_previous_stack_pointer = stack + (capacity - 15);

    return Result::OK;
}

Result Process::initialize_user_stack()
{
    auto user_stack_result = allocate_region(USER_STACK_SIZE, VirtualRegion::Read | VirtualRegion::Write);
    ENSURE(user_stack_result);

    m_user_stack = user_stack_result.value();

    return Result::OK;
}

void Process::new_process_runnable()
{
    auto process = PM.current_process();

    if (process->is_kernel()) {
        process->m_entry_point();
        panic("Kernel process exited, this should not happen!\n");
    } else {
        ASSERT(process->switch_to_user_mode().is_ok());
    }
}

Result Process::switch_to_user_mode()
{
    const u32 kernel_stack_capacity = KERNEL_STACK_SIZE / sizeof(u32);
    const u32 user_stack_capacity = USER_STACK_SIZE / sizeof(u32);

    u32* user_stack = reinterpret_cast<u32*>(m_user_stack->lower().get());
    u32* stack = reinterpret_cast<u32*>(m_kernel_stack->lower().get());

    auto program_region_result = allocate_region(Types::PageSize, VirtualRegion::Read | VirtualRegion::Write);
    ENSURE(program_region_result);

    load_elf();

    // auto program_region = program_region_result.value();
    // program_region->lower().ptr()[0] = 0xeb;
    // program_region->lower().ptr()[1] = 0xfe;

    stack[kernel_stack_capacity - 1] = 0x00DEAD00;                                      // Fallback return address
    stack[kernel_stack_capacity - 2] = CPU::SegmentSelector(CPU::Ring3, 4);             // SS for user mode
    stack[kernel_stack_capacity - 3] = (u32)(user_stack + (kernel_stack_capacity - 4)); // ESP
    stack[kernel_stack_capacity - 4] = 0x0202;                                          // EFLAGS
    stack[kernel_stack_capacity - 5] = CPU::SegmentSelector(CPU::Ring3, 3);             // CS for user mode
    stack[kernel_stack_capacity - 6] = (u32)m_entry_point;                              // EIP

    m_previous_stack_pointer = stack + (kernel_stack_capacity - 6);

    user_stack[user_stack_capacity - 1] = 0xDEAD0000; // Fallback return address
    user_stack[user_stack_capacity - 2] = 0;
    user_stack[user_stack_capacity - 3] = 0;
    user_stack[user_stack_capacity - 4] = 0;

    cli();

    dbgprintf("Process", "Entrypoint: 0x%x\n", m_entry_point);

    CPU::set_ds_register(CPU::SegmentSelector(CPU::Ring3, 4));
    CPU::set_es_register(CPU::SegmentSelector(CPU::Ring3, 4));
    CPU::set_fs_register(CPU::SegmentSelector(CPU::Ring3, 4));
    CPU::set_gs_register(CPU::SegmentSelector(CPU::Ring3, 4));

    start_user_process(m_previous_stack_pointer);

    return Result::OK;
}

Result Process::load_elf()
{
    auto fd_result = VFS::the().open(m_name, 0, 0);
    ENSURE(fd_result);

    auto elf_result = ELF::create(fd_result.value());
    ENSURE(elf_result);
    auto elf_program_headers_result = elf_result.value()->read_program_headers();
    ENSURE(elf_program_headers_result);

    auto& elf_program_headers = elf_program_headers_result.value();
    for (size_t i = 0; i < elf_program_headers.size(); i++) {
        auto program_header = elf_program_headers[i];
        if (program_header.p_type == PT_LOAD) {
            size_t load_location = Types::page_round_down(program_header.p_vaddr);
            size_t load_memory_size = Types::page_round_up(program_header.p_memsz);

            auto region_result = allocate_region_at(load_location, load_memory_size, VirtualRegion::Read | VirtualRegion::Execute);
            ENSURE(region_result);

            dbgprintf("Process", "entry=%x offset=%u filesz=%u\n", elf_result.value()->header().e_entry, program_header.p_offset, program_header.p_filesz);

            fd_result.value()->seek(program_header.p_offset, SEEK_SET);
            fd_result.value()->read(region_result.value()->lower().ptr(), program_header.p_filesz);
        }
    }

    m_entry_point = (void (*)())elf_result.value()->header().e_entry;

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

void Process::dump_stack(bool kernel) const
{
    dbgprintf("Process", "%s Stack\n", kernel ? "Kernel" : "User");
    u32 stack = kernel ? (u32)m_kernel_stack->upper() : (u32)m_user_stack->upper();
    for (int i = 15 * sizeof(u32); i >= 0; i -= sizeof(u32)) {
        dbgprintf("Process", "%x:%x\n", (stack - i), *(u32*)(stack - i));
    }
}
