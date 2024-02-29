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
    stack_allocation = (stack_allocation + Memory::Types::PageSize) & 0xfffffff8;

    m_context = ((Context*)stack_allocation) - 1;

    m_context->m_eflags = 0x0202; // Top of the stack-frame
    m_context->m_cs = 0x08;
    m_context->m_eip = (u32)entry_point;
    m_context->m_ebp = (u32)stack_allocation;
    m_context->m_ebx = 0;
    m_context->m_esi = 0;
    m_context->m_edi = 0; // Bottom of the stack-frame

    dbgprintf("Process", "Process %u (%s) spawned at 0x%x with %u bytes of stack\n", m_pid, m_name, m_entry_point, stack_size);
}

Process::Process(void (*entry_point)(), u32 pid, const char* name, bool is_kernel)
    : Process(entry_point, pid, name, Types::PageSize, is_kernel)
{
}

void Process::dump_stack() const
{
    dbgprintf("Process", "m_context %x\n", m_context);
    u32 context = (u32)m_context;
    for (int i = sizeof(Context); i >= 0; i -= sizeof(u32)) {
        dbgprintf("Process", "%x:%x\n", (context + i), *(u32*)(context + i));
    }
}
