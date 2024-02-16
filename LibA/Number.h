/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

namespace LibA {

template<typename T, typename U>
inline constexpr T ceiling_divide(T a, U b)
{
    T result = a / b;
    if ((a % b) != 0) {
        result++;
    }
    return result;
}

}

using LibA::ceiling_divide;
