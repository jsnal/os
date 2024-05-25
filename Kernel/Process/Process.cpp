#include <Kernel/Assert.h>
#include <Kernel/CPU/CPU.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
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
        m_page_directory->entries()[0].copy(MM.kernel_page_directory().entries()[0]);
        for (u32 page = 768; page < 1024; page++) {
            m_page_directory->entries()[page].copy(MM.kernel_page_directory().entries()[page]);
            m_page_directory->entries()[page].set_user_supervisor(false);
        }
    }
}

ResultReturn<Process*> Process::create_kernel_process(const String& name, void (*entry_point)(), bool add_to_process_list)
{
    pid_t pid = add_to_process_list ? PM.get_next_pid() : 0;
    auto process = new Process(move(name), pid, 0, 0, true);

    auto result = process->initialize_kernel_stack();
    if (result.is_error()) {
        return result;
    }

    process->m_entry_point = entry_point;
    // process->m_context->m_eip = (u32)entry_point;

    if (add_to_process_list) {
        PM.add_process(*process);
    }

    dbgprintf("Process", "Kernel Process '%s' (%u) spawned at 0x%x\n", process->m_name.str(), process->m_pid, entry_point);
    return process;
}

static void test_process()
{
    while (true)
        ;
}

Result Process::create_user_process(const String& path, void (*entry_point)(), uid_t uid, gid_t gid)
{
    auto process = new Process(path, PM.get_next_pid(), uid, gid, false);

    auto result = process->initialize_kernel_stack();
    if (result.is_error()) {
        return result;
    }

    result = process->initialize_user_stack();
    if (result.is_error()) {
        return result;
    }

    process->m_entry_point = entry_point;

    PM.add_process(*process);

    dbgprintf("Process", "User Process '%s' (%u) spawned\n", process->m_name.str(), process->m_pid);
    return Result::OK;
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

    dump_stack(true);

    return Result::OK;
}

Result Process::initialize_user_stack()
{
    m_user_stack = MM.allocate_kernel_region(USER_STACK_SIZE);
    const u32 capacity = KERNEL_STACK_SIZE / sizeof(u32);

    if (m_kernel_stack.ptr() == nullptr) {
        return Result::Failure;
    }

    u32* stack = reinterpret_cast<u32*>(m_user_stack->lower().get());

    stack[capacity - 1] = 0xDEAD0000; // Fallback return address
    stack[capacity - 4] = (u32)m_entry_point;

    return Result::OK;
}

void Process::new_process_runnable()
{
    auto process = PM.current_process();

    if (process->is_kernel()) {
        process->m_entry_point();
        panic("Kernel process exited, this should not happen!\n");
    } else {
        process->switch_to_user_mode();
    }
}

void Process::switch_to_user_mode()
{
    const u32 capacity = KERNEL_STACK_SIZE / sizeof(u32);
    u32* stack = reinterpret_cast<u32*>(m_kernel_stack->lower().get());

    stack[capacity - 2] = CPU::SegmentSelector(CPU::Ring3, 4);    // SS for user mode
    stack[capacity - 3] = m_user_stack->lower() + (capacity - 4); // ESP
    stack[capacity - 4] = 0x0202;                                 // EFLAGS
    stack[capacity - 5] = CPU::SegmentSelector(CPU::Ring3, 3);    // CS for user mode
    stack[capacity - 6] = (u32)m_entry_point;                     // EIP

    m_previous_stack_pointer = stack + (capacity - 6);

    cli();

    dbgprintf("Process", "Entrypoint: 0x%x\n", m_entry_point);

    CPU::set_ds_register(CPU::SegmentSelector(CPU::Ring3, 4));
    CPU::set_es_register(CPU::SegmentSelector(CPU::Ring3, 4));
    CPU::set_fs_register(CPU::SegmentSelector(CPU::Ring3, 4));
    CPU::set_gs_register(CPU::SegmentSelector(CPU::Ring3, 4));

    start_user_process(m_previous_stack_pointer);
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
    // for (int i = 0; i < 10 * sizeof(u32); i += sizeof(u32)) {
    for (int i = 11 * sizeof(u32); i >= 0; i -= sizeof(u32)) {
        dbgprintf("Process", "%x:%x\n", (stack - i), *(u32*)(stack - i));
    }
}
