#include "Kernel/Memory/Types.h"
#include <Kernel/Assert.h>
#include <Kernel/CPU/GDT.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Process/Process.h>
#include <Universal/Stdlib.h>

static u32 s_pid { 0 };
static Process* s_kernel_process;
static LinkedList<Process>* s_processes;

static Process* s_simple_process;

extern "C" void context_switch(u32* old_esp, u32 new_esp);

static void simple_process_run()
{
    dbgprintf("Process", "A simple process!\n");
    dbgprintf("Process", "Kernel esp %x\n", s_kernel_process->esp());
    context_switch(s_simple_process->esp_ptr(), s_kernel_process->esp());
    while (true)
        ;
}

static void idle_process_runnable()
{
    dbgprintf("Process", "Starting the 'idle' process\n");
    while (true)
        ;
}

void Process::create_kernel_process(void (*entry_point)(), const char* name)
{
    auto* kernel_process = new Process(entry_point, name, true);
    s_processes->add_first(kernel_process);
}

void Process::init()
{
    s_processes = new LinkedList<Process>;
    s_kernel_process = new Process(idle_process_runnable, "idle", true);

    s_simple_process = new Process(simple_process_run, "simple", true);

    // GDT::write_tss(&s_kernel_process->m_tss, 0x10, 0x0);
    //
    dbgprintf("Process", "kernel_esp %x, simple_esp %x\n", s_kernel_process->m_esp, s_simple_process->m_esp);

    context_switch(&s_kernel_process->m_esp, s_simple_process->m_esp);

    // load_tss_pointer(0x28);
    // for (auto* p = s_processes->head(); p; p = p->next()) {
    //     dbgprintf("Process", "p: %d %s\n", p->pid(), p->name());
    // }
}

Process::Process(void (*entry_point)(), const char* name, bool is_kernel)
    : m_pid(s_pid++)
    , m_name(name)
    , m_entry_point(entry_point)
    , m_is_kernel(is_kernel)
{
    // User processes not supported... yet
    if (!m_is_kernel) {
        return;
    }

    // m_tss.eflags = 0x0202;
    // m_tss.ds = 0x10;
    // m_tss.es = 0x10;
    // m_tss.fs = 0x10;
    // m_tss.gs = 0x10;
    // m_tss.ss = 0x10;
    // m_tss.cs = 0x08;
    // m_tss.eip = (u32)m_entry_point;

    auto stack_allocation = MemoryManager::the().pmm().kernel_zone().allocate_frame();
    ASSERT(stack_allocation.is_ok());

    memset(stack_allocation.value().ptr(), 0, Memory::Types::PageSize);

    m_esp = Memory::Types::physical_to_virtual(stack_allocation.value());
    u32* stack = (u32*)(m_esp + Memory::Types::PageSize);

    dbgprintf("Process", "Stack top: %x\n", stack);
    // dbgprintf("Process", "offset is %d\n", (u8*)&m_tss.esp0 - (u8*)&m_tss);
    // m_tss.ebp = stack_top;
    // m_tss.esp = stack_top - 16;

    *--stack = 0x00000202;              // eflags
    *--stack = 0x8;                     // cs
    *--stack = (uint32_t)entry_point;   // eip
    *--stack = m_esp + Types::PageSize; // ebp
    *--stack = 0;                       // ebx
    *--stack = 0;                       // esi
    *--stack = 0;                       // edi
    // *--stack = 0;                       // eax
    // *--stack = 0;                       // ecx
    // *--stack = 0;                       // edx
    // *--stack = 0x10;                    // ds
    // *--stack = 0x10;                    // fs
    // *--stack = 0x10;                    // es
    // *--stack = 0x10;                    // gs

    m_esp = (u32)stack;
    m_eip = (u32)entry_point;

    dbgprintf("Process", "Created task %s with esp=0x%x eip=0x%x\n", m_name, m_esp, m_eip);
    // dbgprintf("Process", "Process %u (%s) spawned at 0x%x\n", m_pid, m_name, m_entry_point);
}
