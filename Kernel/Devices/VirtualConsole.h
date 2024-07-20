/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Devices/TTYDevice.h>
#include <Kernel/Graphics/GraphicsManager.h>
#include <Kernel/Graphics/VGATextModeGraphicsCard.h>

class VirtualConsole final : public TTYDevice {
public:
    explicit VirtualConsole(u32 const minor)
        : TTYDevice(4, minor)
    {
        m_graphics = GraphicsManager::the().text_mode_graphics();
    }

    size_t tty_write(const u8* buffer, size_t count) override;

private:
    SharedPtr<VGATextModeGraphicsCard> m_graphics;
};
