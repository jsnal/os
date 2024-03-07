/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/CPU/IRQHandler.h>
#include <Universal/Types.h>

class Keyboard final : public IRQHandler {
public:
    static Keyboard& the();

    Keyboard();

    void handle() override;

    void init();

private:
    u32 get_scan_code();

    void update_modifier(uint8_t modifier, bool pressed);

    u32 m_modifier { 0 };
};
