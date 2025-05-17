/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

namespace Network {

template<typename T>
[[gnu::always_inline]] inline T convert_between_host_and_network(T value)
{
    if constexpr (sizeof(T) == 8) {
        return __builtin_bswap64(value);
    }

    if constexpr (sizeof(T) == 4) {
        return __builtin_bswap32(value);
    }

    if constexpr (sizeof(T) == 2) {
        return __builtin_bswap16(value);
    }

    return value;
}

template<typename T>
class [[gnu::packed]] NetworkEndianness {
public:
    NetworkEndianness() { }
    NetworkEndianness(const T& value)
        : m_value(convert_between_host_and_network(value))
    {
    }

    T host_value() const { return convert_between_host_and_network(m_value); }
    T network_value() const { return m_value; }

private:
    // Value is stored in network endianness (big-endian)
    T m_value { 0 };
};

}
