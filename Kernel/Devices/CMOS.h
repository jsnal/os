/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

class CMOS final {
public:
    static time_t timestamp();

private:
    static void write_register(u8 reg, u8 value);
    static u8 read_register(u8 reg);
};
