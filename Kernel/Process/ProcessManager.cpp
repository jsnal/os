#include <Kernel/Devices/PIT.h>
#include <Kernel/Logger.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/kmalloc.h>

#define QUANTUM_IN_MILLISECONDS 100

u32 ProcessManager::s_current_pid = 0;
Process* ProcessManager::s_current_process = nullptr;
Process* ProcessManager::s_kernel_process = nullptr;
LinkedList<Process>* ProcessManager::s_processes = nullptr;

extern "C" void context_switch(u32* old_esp, u32 new_esp);

static void pit_schedule_wakeup()
{
    ProcessManager::the().schedule();
}

static void kernel_idle_process()
{
    while (true)
        ;
}

ProcessManager& ProcessManager::the()
{
    static ProcessManager s_the;
    return s_the;
}

void ProcessManager::init()
{
    s_current_pid = 0;
    s_processes = new LinkedList<Process>;

    s_kernel_process = new Process(kernel_idle_process, get_next_pid(), "idle", 512, true);

    PIT::register_pit_wakeup(QUANTUM_IN_MILLISECONDS, pit_schedule_wakeup);

    // s_current_process = s_kernel_process;
    // dbgprintf("ProcessManager", "About to context switch to idle\n");
    // dbgprintf("ProcessManager", "s_kernel_process->esp() %x\n", s_kernel_process->esp());
    // context_switch(nullptr, s_kernel_process->esp());
}

void ProcessManager::create_kernel_process(void (*entry_point)(), const char* name)
{
    auto kernel_process = new Process(entry_point, get_next_pid(), name, true);
    s_processes->add_first(kernel_process);
}

void ProcessManager::schedule()
{
    // dbgprintf("ProcessManager", "About to context switch\n");
    // dbgprintf("ProcessManager", "%s: esp=%x\n", s_processes->head()->m_name, s_processes->head()->esp());
    // dbgprintf("ProcessManager", "%s: esp=%x\n", s_processes->tail()->m_name, s_processes->tail()->esp());
    // context_switch(s_processes->head()->esp_ptr(), s_processes->tail()->esp());

    Process* next_process;
    Process* old_process;

    if (s_current_process == nullptr || s_current_process->next() == nullptr) {
        next_process = s_processes->head();
    } else {
        next_process = s_current_process->next();
    }

    if (next_process == nullptr || s_current_process == next_process) {
        dbgprintf("ProcessManager", "unable to schedule\n");
        return;
    }
    old_process = s_current_process;
    s_current_process = next_process;
    // dbgprintf("ProcessManager", "old_process(%s)->m_esp %x\n", old_process->m_name, old_process->esp());
    // dbgprintf("ProcessManager", "next_process(%s)->m_esp %x\n", next_process->m_name, next_process->esp());
    if (old_process == nullptr) {
        context_switch(nullptr, next_process->esp());
    } else {
        context_switch(old_process->esp_ptr(), next_process->esp());
    }
}
