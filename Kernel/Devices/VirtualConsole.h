/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/KeyboardDevice.h>
#include <Kernel/Devices/TTYDevice.h>
#include <Kernel/Graphics/GraphicsManager.h>
#include <Kernel/Graphics/VGATextModeGraphicsCard.h>

class VirtualConsole final : public TTYDevice
    , public KeyboardListener {
public:
    explicit VirtualConsole(u32 const minor)
        : TTYDevice(4, minor)
    {
        m_graphics = GraphicsManager::the().text_mode_graphics();
    }

    virtual ~VirtualConsole() override;

    void set_focused(bool);

    size_t tty_write(const u8* buffer, size_t count) override;
    void tty_echo(char) override;

    void handle_key_event(KeyEvent) override;

private:
    bool m_focused { false };
    SharedPtr<VGATextModeGraphicsCard> m_graphics;
};
