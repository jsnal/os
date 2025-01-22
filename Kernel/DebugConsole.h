/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Graphics/GraphicsManager.h>
#include <stdbool.h>
#include <sys/types.h>

class DebugConsole final {
public:
    static DebugConsole& the();

    DebugConsole();

    bool enable();

    void put_char(char);

    void write(const char* string, size_t length);

private:
    bool m_is_boot_console_enabled { false };
    bool m_is_enabled { false };
};
