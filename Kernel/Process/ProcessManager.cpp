#include <Kernel/Logger.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/kmalloc.h>

u32 ProcessManager::s_current_pid = 0;
Process* ProcessManager::s_current_process = nullptr;
Process* ProcessManager::s_kernel_process = nullptr;
LinkedList<Process>* ProcessManager::s_processes = nullptr;

extern "C" void context_switch(u32* old_esp, u32 new_esp);

ProcessManager& ProcessManager::the()
{
    static ProcessManager s_the;
    return s_the;
}

void ProcessManager::init(void (*entry_point)())
{
    s_current_pid = 0;
    s_processes = new LinkedList<Process>;

    s_kernel_process = new Process(entry_point, get_next_pid(), "idle", 512, true);

    context_switch(nullptr, s_kernel_process->m_esp);
}

void ProcessManager::create_kernel_process(void (*entry_point)(), const char* name)
{
    auto kernel_process = new Process(entry_point, get_next_pid(), name, true);
    s_processes->add_first(kernel_process);
}

void ProcessManager::schedule()
{
    // dbgprintf("ProcessManager", "Scheduling a new process\n");
    // auto* process = s_processes->head();
    //
    // if (s_current_process == process) {
    //     // dbgprintf("ProcessManager", "Ignoring scheduler\n");
    //     return;
    // }
    //
    // s_current_process = process;
    // dbgprintf("ProcessManager", "Calling this one!!\n");
    // context_switch(&s_kernel_process->m_esp, process->m_esp);
}
