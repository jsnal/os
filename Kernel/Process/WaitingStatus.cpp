#include <Kernel/Process/WaitingStatus.h>

void WaitingStatus::set_waiting()
{
    m_is_waiting = true;
};

void WaitingStatus::set_ready()
{
    m_is_waiting = false;
    m_process->set_ready();
};

bool WaitingStatus::is_waiting() const
{
    return m_is_waiting;
};

bool WaitingStatus::is_ready() const
{
    return !m_is_waiting;
};
