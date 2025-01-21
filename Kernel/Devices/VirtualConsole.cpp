/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Devices/VirtualConsole.h>
#include <Kernel/Graphics/EmulatorVGAGraphicsCard.h>
#include <Kernel/Graphics/VGATextModeGraphicsCard.h>

VirtualConsole::~VirtualConsole()
{
}

void VirtualConsole::set_focused(bool focus)
{
    if (!focus) {
        KeyboardDevice::the().set_keyboard_listener(nullptr);
    } else {
        KeyboardDevice::the().set_keyboard_listener(this);
    }
    m_focused = focus;
}

size_t VirtualConsole::tty_write(const u8* buffer, size_t count)
{
    if (m_graphics->is_text_mode()) {
        m_graphics->put_string(reinterpret_cast<const char*>(buffer), count);
    } else {
        dbgprintf("VirtualConsole", "Currently does not support non-text mode devices\n");
    }

    return 0;
}

void VirtualConsole::tty_echo(char c)
{
    tty_write(reinterpret_cast<const u8*>(&c), 1);
}

void VirtualConsole::handle_key_event(KeyEvent event)
{
    dbgprintf("VirtualConsole", "Keyboard event received: %c - %#x, %c - %#x\n", event.character, event.character, event.scan_code, event.scan_code);

    if (event.is_control_pressed() && event.character >= 'a' && event.character <= 'z') {
        handle_input(event.character - 'a' + 1);
    }
    handle_input(event.character);
}
