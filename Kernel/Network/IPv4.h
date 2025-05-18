/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Assert.h>
#include <Universal/BasicString.h>
#include <Universal/Types.h>

namespace Network {

class [[gnu::packed]] IPv4Address {
public:
    static constexpr u8 kLength = 4;

    IPv4Address() = default;

    IPv4Address(const u8 data[kLength])
    {
        m_address[0] = data[0];
        m_address[1] = data[1];
        m_address[2] = data[2];
        m_address[3] = data[3];
    }

    IPv4Address(u8 a, u8 b, u8 c, u8 d)
    {
        m_address[0] = a;
        m_address[1] = b;
        m_address[2] = c;
        m_address[3] = d;
    }

    ~IPv4Address() = default;

    u8 operator[](size_t i) const
    {
        ASSERT(i < kLength);
        return m_address[i];
    }

    const String to_string() const
    {
        return String::format("%u.%u.%u.%u",
            m_address[0], m_address[1], m_address[2], m_address[3]);
    }

private:
    u8 m_address[kLength];
};

}
