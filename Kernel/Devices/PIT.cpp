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

#define WAKEUP_ROUTINES_SIZE 2

static u32 s_milliseconds_since_boot = 0;
static u32 s_seconds_since_boot = 0;
static u8 s_wakeup_routine_count = 0;
static PIT::WakeupRoutine s_wakeup_routines[WAKEUP_ROUTINES_SIZE];

static void pit_interrupt_handler(InterruptFrame*)
{
    s_milliseconds_since_boot++;
    if (s_milliseconds_since_boot % TICKS_PER_SECOND == 0) {
        s_seconds_since_boot++;
    }

    for (int i = 0; i < s_wakeup_routine_count; i++) {
        if (s_wakeup_routines[i].wakeup_routine != nullptr && s_milliseconds_since_boot % s_wakeup_routines[i].milliseconds == 0) {
            s_wakeup_routines[i].wakeup_routine();
        }
    }
}

namespace PIT {

void register_pit_wakeup(u32 milliseconds, void (*wakeup_routine_pointer)())
{
    if (s_wakeup_routine_count >= WAKEUP_ROUTINES_SIZE || wakeup_routine_pointer == nullptr) {
        return;
    }

    dbgprintf("PIT", "Registering a PIT wakeup @ %x every %d ms\n", wakeup_routine_pointer, milliseconds);
    s_wakeup_routines[s_wakeup_routine_count].milliseconds = milliseconds;
    s_wakeup_routines[s_wakeup_routine_count++].wakeup_routine = wakeup_routine_pointer;
}

uint32_t milliseconds_since_boot()
{
    return s_milliseconds_since_boot;
}

uint32_t seconds_since_boot()
{
    return s_seconds_since_boot;
}

void init()
{
    uint16_t timer_reload;

    IO::outb(PIT_COMMAND, PIT_SELECT_CHANNEL_0 | PIT_WRITE_WORD | PIT_MODE_SQUARE_WAVE);

    timer_reload = PIT_BASE_FREQUENCY / TICKS_PER_SECOND;

    dbgprintf("PIT", "Initialized PIT: %u Hz, square wave (0x%x)\n", TICKS_PER_SECOND, timer_reload);

    IO::outb(PIT_CHANNEL_0, timer_reload & 0xFF);        // Low byte
    IO::outb(PIT_CHANNEL_0, (timer_reload >> 8) & 0xFF); // High byte

    IDT::register_interrupt_handler(ISR_PIT, pit_interrupt_handler);
}

}
