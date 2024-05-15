/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Graphics/GraphicsManager.h>
#include <stdbool.h>
#include <sys/types.h>

class Console final {
public:
    static Console& the();

    Console();

    bool enable();

    void put_char(char);

    void write(const char* string, size_t length);

    void disable_boot_console() { m_boot_console.clear(); }
    void enable_boot_console() { m_boot_console = GraphicsManager::init_boot_console(); }

private:
    SharedPtr<VGATextModeGraphicsCard> m_boot_console;

    bool m_is_boot_console_enabled { false };
    bool m_is_enabled { false };
};
