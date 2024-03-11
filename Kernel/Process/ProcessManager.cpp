#include <Kernel/CPU/IDT.h>
#include <Kernel/CPU/PIC.h>
#include <Kernel/Devices/PIT.h>
#include <Kernel/Logger.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/kmalloc.h>

#define QUANTUM_IN_MILLISECONDS 100

pid_t ProcessManager::s_current_pid;
Process* ProcessManager::s_current_process;
Process* ProcessManager::s_kernel_process;
LinkedList<Process>* ProcessManager::s_processes;

extern "C" void context_run(Process::Context* context);

extern "C" void context_switch(Process::Context** old_context, Process::Context** new_context);
extern "C" void context_switch_to_created(Process::Context** old_context, Process::Context** new_context);

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
    PIT::the().register_pit_wakeup(QUANTUM_IN_MILLISECONDS, pit_schedule_wakeup);

    s_current_process = s_kernel_process;
    context_run(s_kernel_process->m_context);
}

void ProcessManager::create_kernel_process(void (*entry_point)(), const char* name)
{
    auto kernel_process = new Process(entry_point, get_next_pid(), name, true);
    s_processes->add_first(kernel_process);
}

ResultOr<Process*> ProcessManager::find_by_pid(pid_t pid) const
{
    for (Process* p = s_processes->head(); p != nullptr; p = p->next()) {
        if (p->pid() == pid) {
            return p;
        }
    }
    return {};
}

void ProcessManager::schedule()
{
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

        dbgprintf_if(DEBUG_PROCESS_MANAGER, "ProcessManager", "Process '%s' state %d\n", p->name(), p->state());
        if (p->state() == Process::Ready || p->state() == Process::Created) {
            next_process = p;
            break;
        }
    }
    dbgprintf_if(DEBUG_PROCESS_MANAGER, "ProcessManager", "Picked Process '%s'\n", p->name());

    if (next_process == nullptr) {
        s_current_process = s_kernel_process;
        context_switch(previous_process->context_ptr(), s_kernel_process->context_ptr());
        return;
    }

    previous_process->set_state(Process::Ready);
    s_current_process = next_process;

    if (next_process->state() == Process::Created) {
        next_process->set_state(Process::Running);
        context_switch_to_created(previous_process->context_ptr(), next_process->context_ptr());
    } else {
        next_process->set_state(Process::Running);
        context_switch(previous_process->context_ptr(), next_process->context_ptr());
    }
}

void ProcessManager::yield()
{
    // TODO: This should check for more conditions
    enter_critical();
    schedule();
    exit_critical();
}

void ProcessManager::enter_critical()
{
    cli();
    if ((u32)m_critical_count + 1 > U8_MAX) {
        panic("Too many critical sections!\n");
    }
    m_critical_count++;
}

void ProcessManager::exit_critical()
{
    if (--m_critical_count == 0) {
        sti();
    }
}
