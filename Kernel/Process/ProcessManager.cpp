#include <Kernel/CPU/CPU.h>
#include <Kernel/CPU/GDT.h>
#include <Kernel/CPU/IDT.h>
#include <Kernel/CPU/PIC.h>
#include <Kernel/Devices/PIT.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/Paging.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/kmalloc.h>

#define QUANTUM_IN_MILLISECONDS 50

extern "C" void context_run(Process::Context* context);
extern "C" void context_switch(Process::Context** old_context, Process::Context** new_context, u32 cr3);
extern "C" void context_switch_to_created(Process::Context** old_context, Process::Context** new_context, u32 cr3);

static void pit_schedule_wakeup()
{
    ProcessManager::the().schedule();
}

static void kernel_idle_process()
{
    dbgprintf("ProcessManager", "Starting the idle process!\n");
    while (true)
        ;
}

ProcessManager& ProcessManager::the()
{
    static ProcessManager m_the;
    return m_the;
}

ProcessManager::ProcessManager()
    : m_processes(new LinkedList<Process>)
{
    memset(&m_tss, 0, sizeof(TSS));
    m_tss.ss0 = CPU::SegmentSelector(CPU::Ring0, 2);
    m_tss.esp0 = 0x1337;
    m_tss.cs = CPU::SegmentSelector(CPU::Ring3, 1);
    m_tss.ss = CPU::SegmentSelector(CPU::Ring3, 2);
    m_tss.ds = CPU::SegmentSelector(CPU::Ring3, 2);
    m_tss.es = CPU::SegmentSelector(CPU::Ring3, 2);
    m_tss.fs = CPU::SegmentSelector(CPU::Ring3, 2);
    m_tss.gs = CPU::SegmentSelector(CPU::Ring3, 2);

    GDT::write_tss(&m_tss);

    auto idle_process_result = Process::create_kernel_process("idle", kernel_idle_process, false);
    ASSERT(idle_process_result.is_ok());
    m_kernel_idle_process = idle_process_result.value();
}

void ProcessManager::start()
{
    // PIT::the().register_pit_wakeup(QUANTUM_IN_MILLISECONDS, pit_schedule_wakeup);

    // m_current_process = m_kernel_idle_process;
    // start_kernel_process(m_kernel_idle_process->previous_stack_pointer());
    dbgprintf("ProcessManager", "name %s\n", m_processes->head()->name().str());

    m_current_process = m_processes->head();
    start_kernel_process(m_current_process->previous_stack_pointer());
    // context_run(m_kernel_idle_process->m_context);
}

void ProcessManager::add_process(Process& process)
{
    m_processes->add_first(&process);
}

ResultReturn<Process*> ProcessManager::find_by_pid(pid_t pid) const
{
    for (Process* p = m_processes->head(); p != nullptr; p = p->next()) {
        if (p->pid() == pid) {
            return p;
        }
    }
    return {};
}

void ProcessManager::schedule()
{
    Process* next_process = nullptr;
    Process* previous_process = m_current_process;
    Process* p = m_current_process;

    if (m_current_process == m_kernel_idle_process) {
        p = m_processes->head();
    }

    for (u32 i = 0; i < m_processes->size(); i++) {
        if (p->next() == nullptr) {
            p = m_processes->head();
        } else {
            p = p->next();
        }

        dbgprintf_if(DEBUG_PROCESS_MANAGER, "ProcessManager", "Process '%s' state %d\n", p->name().str(), p->state());
        if (p->state() == Process::Ready || p->state() == Process::Created) {
            next_process = p;
            break;
        }
    }

    if (next_process == nullptr) {
        next_process = m_kernel_idle_process;
    }

    dbgprintf_if(DEBUG_PROCESS_MANAGER, "ProcessManager", "Picked Process '%s'\n", next_process->name().str());

    previous_process->set_state(Process::Ready);
    m_current_process = next_process;

    if (next_process->state() == Process::Created) {
        next_process->set_state(Process::Running);
        context_switch_to_created(previous_process->context_ptr(), next_process->context_ptr(), next_process->page_directory().base());
    } else {
        next_process->set_state(Process::Running);
        context_switch(previous_process->context_ptr(), next_process->context_ptr(), next_process->page_directory().base());
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
