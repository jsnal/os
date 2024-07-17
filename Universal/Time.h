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

    time_t localtime() const
    {
        return days_since_epoch() * 86400
            + days_since_start_of_year() * 86400
            + (m_day - 1) * 86400
            + m_hour * 3600
            + m_minute * 60
            + m_second;
    }

private:
    u32 days_since_epoch() const
    {
        u32 days = 0;
        for (int i = 1969; i < (m_year - 1); i++) {
            days += 365;
            if (is_leap_year(i)) {
                days++;
            }
        }
        return days;
    }

    u32 days_since_start_of_year() const
    {
        u32 days = 0;
        switch (m_month - 1) {
            case 11:
                days += 30;
                [[fallthrough]];
            case 10:
                days += 31;
                [[fallthrough]];
            case 9:
                days += 30;
                [[fallthrough]];
            case 8:
                days += 31;
                [[fallthrough]];
            case 7:
                days += 31;
                [[fallthrough]];
            case 6:
                days += 30;
                [[fallthrough]];
            case 5:
                days += 31;
                [[fallthrough]];
            case 4:
                days += 30;
                [[fallthrough]];
            case 3:
                days += 31;
                [[fallthrough]];
            case 2:
                if (is_leap_year(m_year)) {
                    days += 29;
                } else {
                    days += 28;
                }
                [[fallthrough]];
            case 1:
                days += 31;
        }
        return days;
    }

    bool is_leap_year(u16 year) const
    {
        return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
    }

    u8 m_second { 0 };
    u8 m_minute { 0 };
    u8 m_hour { 0 };
    u8 m_day { 0 };
    u8 m_month { 0 };
    u16 m_year { 0 };
};
