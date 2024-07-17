/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Time.h>
#include <Universal/Types.h>

class CMOS final {
public:
    static const Time& boot_time();
    static void time(Time&);

private:
    static bool is_leap_year(u16 year);
    static u32 days_since_epoch(u16 year);
    static u32 days_since_start_of_year(u8 month, u16 year);
    static void write_register(u8 reg, u8 value);
    static u8 read_register(u8 reg);
};
