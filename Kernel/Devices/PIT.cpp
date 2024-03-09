/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/CPU/IDT.h>
#include <Kernel/Devices/PIT.h>
#include <Kernel/IO.h>
#include <Kernel/Logger.h>
#include <Kernel/Process/ProcessManager.h>

#define PIT_BASE_FREQUENCY 1193182

#define PIT_CHANNEL_0 0x40
#define PIT_CHANNEL_2 0x42
#define PIT_COMMAND 0x43

#define PIT_SELECT_CHANNEL_0 0x00
#define PIT_SELECT_CHANNEL_2 0x80

#define PIT_MODE_SQUARE_WAVE 0x06

#define PIT_WRITE_WORD 0x30

u32 PIT::s_milliseconds_since_boot = 0;
u32 PIT::s_seconds_since_boot = 0;

PIT& PIT::the()
{
    static PIT s_the;
    return s_the;
}

PIT::PIT()
    : IRQHandler(IRQ_PIT)
{
    IO::outb(PIT_COMMAND, PIT_SELECT_CHANNEL_0 | PIT_WRITE_WORD | PIT_MODE_SQUARE_WAVE);

    u16 timer_reload = PIT_BASE_FREQUENCY / TICKS_PER_SECOND;

    IO::outb(PIT_CHANNEL_0, timer_reload & 0xFF);        // Low byte
    IO::outb(PIT_CHANNEL_0, (timer_reload >> 8) & 0xFF); // High byte

    enable_irq();

    dbgprintf("PIT", "Initialized PIT: %u Hz, square wave (0x%x)\n", TICKS_PER_SECOND, timer_reload);
}

void PIT::handle()
{
    s_milliseconds_since_boot++;
    if (s_milliseconds_since_boot % TICKS_PER_SECOND == 0) {
        s_seconds_since_boot++;
    }

    // NOTE: Go ahead and send the end-of-interrupt just in case a process is about to yield, in
    //       which case it would never be sent and the system would hang.
    eoi();

    for (int i = 0; i < m_wakeup_routine_count; i++) {
        if (m_wakeup_routines[i].wakeup_routine != nullptr && s_milliseconds_since_boot % m_wakeup_routines[i].milliseconds == 0) {
            m_wakeup_routines[i].wakeup_routine();
        }
    }
}

void PIT::register_pit_wakeup(u32 milliseconds, void (*wakeup_routine_pointer)())
{
    if (m_wakeup_routine_count >= WAKEUP_ROUTINES_COUNT || wakeup_routine_pointer == nullptr) {
        return;
    }

    dbgprintf("PIT", "Registering a PIT wakeup @ %x every %d ms\n", wakeup_routine_pointer, milliseconds);
    m_wakeup_routines[m_wakeup_routine_count].milliseconds = milliseconds;
    m_wakeup_routines[m_wakeup_routine_count++].wakeup_routine = wakeup_routine_pointer;
}
