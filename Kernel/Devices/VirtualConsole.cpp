/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Devices/VirtualConsole.h>
#include <Kernel/Graphics/EmulatorVGAGraphicsCard.h>
#include <Kernel/Graphics/VGATextModeGraphicsCard.h>

size_t VirtualConsole::tty_write(const u8* buffer, size_t count)
{
    if (m_graphics->is_text_mode()) {
        dbgprintf("VirtualConsole", "Writing a string\n");
        m_graphics->put_string(reinterpret_cast<const char*>(buffer));
    } else {
        dbgprintf("VirtualConsole", "Currently does not support non-text mode devices\n");
    }

    return 0;
}
