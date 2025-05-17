/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Array.h>
#include <Universal/BasicString.h>
#include <Universal/Stdlib.h>
#include <Universal/Types.h>

namespace Network {

class [[gnu::packed]] MACAddress {
public:
    static constexpr u8 kLength = 6;

    MACAddress() = default;

    MACAddress(const u8 data[kLength])
    {
        m_address[0] = data[0];
        m_address[1] = data[1];
        m_address[2] = data[2];
        m_address[3] = data[3];
        m_address[4] = data[4];
        m_address[5] = data[5];
    }

    MACAddress(u8 a, u8 b, u8 c, u8 d, u8 e, u8 f)
    {
        m_address[0] = a;
        m_address[1] = b;
        m_address[2] = c;
        m_address[3] = d;
        m_address[4] = e;
        m_address[5] = f;
    }

    ~MACAddress() = default;

    u8 operator[](size_t i) const
    {
        ASSERT(i < kLength);
        return m_address[i];
    }

    const String to_string() const
    {
        return String::format("%02x:%02x:%02x:%02x:%02x:%02x",
            m_address[0], m_address[1], m_address[2], m_address[3], m_address[4], m_address[5]);
    }

private:
    u8 m_address[kLength];
};

}
