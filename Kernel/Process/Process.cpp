#include "Kernel/Memory/Types.h"
#include <Kernel/Assert.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Process/ProcessManager.h>
#include <Universal/Stdlib.h>

Process::Process(void (*entry_point)(), pid_t pid, String&& name)
    : m_entry_point(entry_point)
    , m_name(move(name))
    , m_pid(pid)
    , m_is_kernel(true)
    , m_state(State::Created)
{
    m_page_directory = MM.kernel_page_directory();
}

Process::Process(void (*entry_point)(), pid_t pid, uid_t uid, gid_t gid, String&& name)
    : m_entry_point(entry_point)
    , m_name(move(name))
    , m_pid(pid)
    , m_uid(uid)
    , m_gid(gid)
    , m_is_kernel(false)
    , m_state(State::Created)
{
    m_page_directory = PageDirectory::create_user_page_directory(*this);
    m_page_directory->set_base(MM.allocate_physical_kernel_page());
    m_page_directory->entries()[0].copy(MM.kernel_page_directory().entries()[0]);
    for (u32 page = 768; page < 1024; page++) {
        m_page_directory->entries()[page].copy(MM.kernel_page_directory().entries()[page]);
    }
}

ResultOr<Process*> Process::create_standalone_kernel_process(void (*entry_point)(), String&& name, pid_t pid, size_t stack_size)
{
    auto process = new Process(entry_point, pid, move(name));

    auto result = process->initialize_stack(stack_size);
    if (result.is_error()) {
        return result;
    }

    dbgprintf("Process", "Kernel Process %u (%s) spawned at 0x%x with %u bytes of stack\n", process->m_pid, process->m_name.str(), entry_point, stack_size);
    return process;
}

Result Process::create_kernel_process(void (*entry_point)(), String&& name)
{
    auto process = new Process(entry_point, PM.get_next_pid(), move(name));

    auto result = process->initialize_stack(Types::PageSize);
    if (result.is_error()) {
        return result;
    }

    PM.add_process(*process);

    dbgprintf("Process", "Kernel Process %u (%s) spawned at 0x%x\n", process->m_pid, process->m_name.str(), entry_point);
    return Result::OK;
}

Result Process::create_user_process(void (*entry_point)(), uid_t uid, gid_t gid, String&& name)
{
    auto process = new Process(entry_point, PM.get_next_pid(), uid, gid, move(name));

    auto result = process->initialize_stack(Types::PageSize);
    if (result.is_error()) {
        return result;
    }

    PM.add_process(*process);

    dbgprintf("Process", "User Process %u (%s) spawned at 0x%x\n", process->m_pid, process->m_name.str(), entry_point);
    return Result::OK;
}

Result Process::initialize_stack(size_t size)
{
    u32* stack_allocation = 0;
    if (size == Types::PageSize) {
        auto region = MemoryManager::the().allocate_physical_kernel_page();
        stack_allocation = (u32*)Memory::Types::physical_to_virtual(region);
    } else {
        stack_allocation = (u32*)kmalloc(size);
    }

    if (stack_allocation == nullptr) {
        return Result::Failure;
    }

    memset(stack_allocation, 0, Memory::Types::PageSize);
    stack_allocation = (stack_allocation + Memory::Types::PageSize);

    m_context = ((Context*)stack_allocation) - 1;

    m_context->m_eflags = 0x0202; // Top of the stack-frame
    m_context->m_cs = 0x08;
    m_context->m_eip = (u32)m_entry_point;
    m_context->m_ebp = (u32)stack_allocation;
    m_context->m_ebx = 0;
    m_context->m_esi = 0;
    m_context->m_edi = 0; // Bottom of the stack-frame

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

void Process::dump_stack() const
{
    dbgprintf("Process", "m_context %x\n", m_context);
    u32 context = (u32)m_context;
    for (int i = sizeof(Context); i >= 0; i -= sizeof(u32)) {
        dbgprintf("Process", "%x:%x\n", (context + i), *(u32*)(context + i));
    }
}
