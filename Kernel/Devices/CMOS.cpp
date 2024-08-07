/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Devices/CMOS.h>
#include <Kernel/IO.h>

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define CMOS_REGISTER_SECOND 0x00
#define CMOS_REGISTER_MINUTE 0x02
#define CMOS_REGISTER_HOUR 0x04
#define CMOS_REGISTER_DAY 0x07
#define CMOS_REGISTER_MONTH 0x08
#define CMOS_REGISTER_YEAR 0x09

#define CMOS_STATUS_A_REGISTER 0x0A
#define CMOS_STATUS_B_REGISTER 0x0B
#define CMOS_STATUS_UPDATE_IN_PROGRESS 0x80

// Binary coded decimal conversion
#define BCD(value) ((value & 0x0F) + ((value / 16) * 10))

static Time s_boot_time = {};

const Time& CMOS::boot_time()
{
    if (s_boot_time.year() == 0) {
        time(s_boot_time);
    }
    return s_boot_time;
}

void CMOS::time(Time& time)
{
    while (read_register(CMOS_STATUS_A_REGISTER) & CMOS_STATUS_UPDATE_IN_PROGRESS)
        ;

    u8 second = read_register(CMOS_REGISTER_SECOND);
    u8 minute = read_register(CMOS_REGISTER_MINUTE);
    u8 hour = read_register(CMOS_REGISTER_HOUR);
    u8 day = read_register(CMOS_REGISTER_DAY);
    u8 month = read_register(CMOS_REGISTER_MONTH);
    u16 year = read_register(CMOS_REGISTER_YEAR);

    u8 status_b = read_register(CMOS_STATUS_B_REGISTER);

    if (!(status_b & 0x04)) {
        second = BCD(second);
        minute = BCD(minute);
        hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
        day = BCD(day);
        month = BCD(month);
        year = BCD(year);
    }

    if (!(status_b & 0x02) && (hour & 0x80)) {
        hour = ((hour & 0x7F) + 12) % 24;
    }

    // Stupid way of doing this but it works for longer than I will be alive, so I don't care.
    if (year > 23 && year < 99) {
        year += 2000;
    } else {
        year += 2100;
    }

    time = Time(second, minute, hour, day, month, year);
}

void CMOS::write_register(u8 reg, u8 value)
{
    IO::outb(CMOS_ADDRESS, reg);
    IO::outb(CMOS_DATA, value);
}

u8 CMOS::read_register(u8 reg)
{
    IO::outb(CMOS_ADDRESS, reg);
    return IO::inb(CMOS_DATA);
}
