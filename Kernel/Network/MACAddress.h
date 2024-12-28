/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Stdlib.h>
#include <Universal/String.h>
#include <Universal/Types.h>

namespace Network {

class [[gnu::packed]] MACAddress {
public:
    static constexpr u8 kLength = 8;

    MACAddress() = default;

    MACAddress(const u8 data[kLength])
    {
        memcpy(m_data, data, kLength);
    }

    MACAddress(u8 a, u8 b, u8 c, u8 d, u8 e, u8 f)
    {
        m_data[0] = a;
        m_data[1] = b;
        m_data[2] = c;
        m_data[3] = d;
        m_data[4] = e;
        m_data[5] = f;
    }

    ~MACAddress() = default;

    const u8* get() const { return m_data; }

    const String to_string() const
    {
        return String::format("%02x:%02x:%02x:%02x:%02x:%02x",
            m_data[0], m_data[1], m_data[2], m_data[3], m_data[4], m_data[5]);
    }

private:
    u8 m_data[kLength];
};

}
