#include "Kernel/Memory/Types.h"
#include <Kernel/Assert.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Process/Process.h>
#include <Universal/Stdlib.h>

Process::Process(void (*entry_point)(), u32 pid, const char* name, size_t stack_size, bool is_kernel)
    : m_pid(pid)
    , m_name(name)
    , m_entry_point(entry_point)
    , m_is_kernel(is_kernel)
{
    // User processes not supported... yet
    if (!m_is_kernel) {
        return;
    }

    u32 stack_allocation = 0;
    if (stack_size == Types::PageSize) {
        auto frame = MemoryManager::the().pmm().kernel_zone().allocate_frame();
        ASSERT(frame.is_ok());
        stack_allocation = Memory::Types::physical_to_virtual(frame.value());
    } else {
        stack_allocation = (u32)kmalloc(stack_size);
    }

    memset((u32*)stack_allocation, 0, Memory::Types::PageSize);

    m_esp = stack_allocation;
    u32* stack = (u32*)(m_esp + Memory::Types::PageSize);

    *(--stack) = 0x0202;                          // eflags
    *(--stack) = 0x8;                             // cs
    *(--stack) = (uint32_t)entry_point;           // eip
    *(--stack) = m_ebp = m_esp + Types::PageSize; // ebp
    *(--stack) = 0;                               // ebx
    *(--stack) = 0;                               // esi
    *(--stack) = 0;                               // edi

    m_esp = (u32)stack;
    m_eip = (u32)entry_point;

    dbgprintf("Process", "Process %u (%s) spawned at 0x%x with %u bytes of stack\n", m_pid, m_name, m_entry_point, stack_size);
}

Process::Process(void (*entry_point)(), u32 pid, const char* name, bool is_kernel)
    : Process(entry_point, pid, name, Types::PageSize, is_kernel)
{
}
