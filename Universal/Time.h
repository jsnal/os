/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

class Time {
public:
    Time() { }

    Time(u8 second, u8 minute, u8 hour, u8 day, u8 month, u16 year)
        : m_second(second)
        , m_minute(minute)
        , m_hour(hour)
        , m_day(day)
        , m_month(month)
        , m_year(year)
    {
    }

    u8 second() const { return m_second; }
    u8 minute() const { return m_minute; }
    u8 hour() const { return m_hour; }
    u8 day() const { return m_day; }
    u8 month() const { return m_month; }
    u16 year() const { return m_year; }

    time_t localtime() const;

    const char* date_string(bool twelve_hour_format) const;
    const char* month_string() const;
    const char* day_string() const;

private:
    u32 days_since_epoch() const;
    u32 days_since_start_of_year() const;
    bool is_leap_year(u16 year) const;

    u8 m_second { 0 };
    u8 m_minute { 0 };
    u8 m_hour { 0 };
    u8 m_day { 0 };
    u8 m_month { 0 };
    u16 m_year { 0 };
};
