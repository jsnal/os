#include <Kernel/Devices/PIT.h>
#include <Kernel/Logger.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/kmalloc.h>

#define QUANTUM_IN_MILLISECONDS 100

u32 ProcessManager::s_current_pid;
Process* ProcessManager::s_current_process;
Process* ProcessManager::s_kernel_process;
LinkedList<Process>* ProcessManager::s_processes;

extern "C" void context_run(Process::Context* context);

extern "C" void context_switch(Process::Context** old_context, Process::Context* new_context);

static void pit_schedule_wakeup()
{
    ProcessManager::the().schedule();
}

static void kernel_idle_process()
{
    dbgprintf("ProcessManager", "Starting the idle process!\n");
    ProcessManager::the().schedule();
    while (true)
        ;
}

ProcessManager& ProcessManager::the()
{
    static ProcessManager s_the;
    return s_the;
}

ProcessManager::ProcessManager()
{
    s_current_pid = 0;
    s_current_process = nullptr;
    s_kernel_process = new Process(kernel_idle_process, get_next_pid(), "idle", 512, true);
    s_processes = new LinkedList<Process>;
}

void ProcessManager::init()
{
    PIT::register_pit_wakeup(QUANTUM_IN_MILLISECONDS, pit_schedule_wakeup);

    s_current_process = s_kernel_process;
    context_run(s_kernel_process->m_context);
}

void ProcessManager::create_kernel_process(void (*entry_point)(), const char* name)
{
    auto kernel_process = new Process(entry_point, get_next_pid(), name, true);
    kernel_process->set_state(Process::Ready);
    s_processes->add_first(kernel_process);
}

void ProcessManager::schedule()
{

    dbgprintf("ProcessManager", "Running the scheduler\n");
    Process* next_process = nullptr;
    Process* previous_process = s_current_process;
    Process* p = s_current_process;

    if (s_current_process == s_kernel_process) {
        p = s_processes->head();
    }

    for (u32 i = 0; i < s_processes->size(); i++) {
        if (p->next() == nullptr) {
            p = s_processes->head();
        } else {
            p = p->next();
        }

        if (p->state() == Process::Ready) {
            next_process = p;
            break;
        }
    }

    if (next_process == nullptr) {
        dbgprintf("ProcessManager", "No processes to run, falling back to idle\n");
        if (s_current_process == s_kernel_process) {
            return;
        }

        s_current_process = s_kernel_process;
        context_switch(previous_process->context_ptr(), s_kernel_process->context());
        return;
    }

    s_current_process = next_process;
    context_switch(previous_process->context_ptr(), next_process->context());
}
