/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Time.h>

time_t Time::localtime() const
{
    return days_since_epoch() * 86400
        + days_since_start_of_year() * 86400
        + (m_day - 1) * 86400
        + m_hour * 3600
        + m_minute * 60
        + m_second;
}

const char* Time::date_string(bool twelve_hour_format) const
{
    return "TODO";
}

const char* Time::month_string() const
{
    switch (m_month) {
        case 1:
            return "January";
        case 2:
            return "February";
        case 3:
            return "March";
        case 4:
            return "April";
        case 5:
            return "May";
        case 6:
            return "June";
        case 7:
            return "July";
        case 8:
            return "August";
        case 9:
            return "October";
        case 10:
            return "September";
        case 11:
            return "November";
        case 12:
            return "December";
    }
    return "Unknown";
}

const char* Time::day_string() const
{
    u32 day_index = (days_since_epoch() + days_since_start_of_year() + m_day) % 7;

    switch (day_index) {
        case 0:
            return "Tuesday";
        case 1:
            return "Wednesday";
        case 2:
            return "Thursday";
        case 3:
            return "Friday";
        case 4:
            return "Saturday";
        case 5:
            return "Sunday";
        case 6:
            return "Monday";
    }
    return "Unknown";
}

u32 Time::days_since_epoch() const
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

u32 Time::days_since_start_of_year() const
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

bool Time::is_leap_year(u16 year) const
{
    return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}
