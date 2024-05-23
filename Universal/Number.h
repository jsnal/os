/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

namespace Universal {

template<typename T, typename U>
inline constexpr bool number_between(T number, U a, U b)
{
    return number > a && number < b;
}

template<typename T, typename U>
inline constexpr bool number_between_inclusive(T number, U a, U b)
{
    return number >= a && number <= b;
}

template<typename T, typename U>
inline constexpr T ceiling_divide(T a, U b)
{
    T result = a / b;
    if ((a % b) != 0) {
        result++;
    }
    return result;
}

template<typename T>
inline constexpr T min(T a, T b)
{
    if (a <= b) {
        return a;
    } else {
        return b;
    }
}

}

using Universal::ceiling_divide;
using Universal::min;
using Universal::number_between;
using Universal::number_between_inclusive;
