/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Logger.h>
#include <Kernel/TTY/VirtualConsole.h>

#define VGA_BUFFER 0xc03ff000

static u8* s_vga_buffer;
static u16 s_active_console;

VirtualConsole::VirtualConsole(u16 index)
    : m_index(index)
{
}

void VirtualConsole::init()
{
    dbgprintf("VirtualConsole", "VGA Text Mode console initialized @ 0x%x", VGA_BUFFER);
    s_vga_buffer = (u8*)VGA_BUFFER;
    s_active_console = -1;
}
