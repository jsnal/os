/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <cpu/idt.h>
#include <devices/pit.h>
#include <logger.h>
#include <sys/io.h>

#define DEBUG_TAG "PIT"

#define PIT_BASE_FREQUENCY 1193182

#define PIT_CHANNEL_0 0x40
#define PIT_CHANNEL_2 0x42
#define PIT_COMMAND 0x43

#define PIT_SELECT_CHANNEL_0 0x00
#define PIT_SELECT_CHANNEL_2 0x80

#define PIT_MODE_SQUARE_WAVE 0x06

#define PIT_WRITE_WORD 0x30

uint32_t milliseconds_since_boot = 0;
uint32_t seconds_since_boot = 0;

static void pit_isr_handler()
{
    milliseconds_since_boot++;
    if (milliseconds_since_boot % TICKS_PER_SECOND == 0) {
        seconds_since_boot++;
    }
}

void pit_init()
{
    uint16_t timer_reload;

    outb(PIT_COMMAND, PIT_SELECT_CHANNEL_0 | PIT_WRITE_WORD | PIT_MODE_SQUARE_WAVE);

    timer_reload = PIT_BASE_FREQUENCY / TICKS_PER_SECOND;

    dbgprintf("Initialized PIT: %u Hz, square wave (0x%x)\n", TICKS_PER_SECOND, timer_reload);

    outb(PIT_CHANNEL_0, timer_reload & 0xFF);        // Low byte
    outb(PIT_CHANNEL_0, (timer_reload >> 8) & 0xFF); // High byte

    isr_register_handler(ISR_PIT, pit_isr_handler);
}
