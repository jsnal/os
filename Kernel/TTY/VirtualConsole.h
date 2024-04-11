/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

class VirtualConsole {
public:
    static void init();

    VirtualConsole(u16 index);

private:
    u16 m_index;
};
