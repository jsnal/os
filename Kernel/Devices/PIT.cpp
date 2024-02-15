/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <CPU/IDT.h>
#include <Devices/PIT.h>
#include <IO.h>
#include <Logger.h>

#define DEBUG_TAG "PIT"

#define PIT_BASE_FREQUENCY 1193182

#define PIT_CHANNEL_0 0x40
#define PIT_CHANNEL_2 0x42
#define PIT_COMMAND 0x43

#define PIT_SELECT_CHANNEL_0 0x00
#define PIT_SELECT_CHANNEL_2 0x80

#define PIT_MODE_SQUARE_WAVE 0x06

#define PIT_WRITE_WORD 0x30

static uint32_t s_milliseconds_since_boot = 0;
static uint32_t s_seconds_since_boot = 0;

static void pit_interrupt_handler()
{
    s_milliseconds_since_boot++;
    if (s_milliseconds_since_boot % TICKS_PER_SECOND == 0) {
        s_seconds_since_boot++;
    }
}

namespace PIT {

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

    dbgprintf("Initialized PIT: %u Hz, square wave (0x%x)\n", TICKS_PER_SECOND, timer_reload);

    IO::outb(PIT_CHANNEL_0, timer_reload & 0xFF);        // Low byte
    IO::outb(PIT_CHANNEL_0, (timer_reload >> 8) & 0xFF); // High byte

    IDT::register_interrupt_handler(ISR_PIT, pit_interrupt_handler);
}

}
