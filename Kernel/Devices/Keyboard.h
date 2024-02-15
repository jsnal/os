/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

class Keyboard final {
public:
    static Keyboard& the();

    Keyboard();

    void init();

    void handle_interrupt();

private:
    uint32_t get_scan_code();

    void update_modifier(uint8_t modifier, bool pressed);

    uint32_t m_modifier { 0 };
};
