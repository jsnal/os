/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Array.h>
#include <Universal/Stdlib.h>
#include <Universal/String.h>
#include <Universal/Types.h>

namespace Network {

class MACAddress {
public:
    static constexpr u8 kLength = 8;

    MACAddress() = default;

    MACAddress(const u8 data[kLength])
    {
        memcpy(m_address.ptr(), data, kLength);
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

    const Array<u8, kLength>& address() const { return m_address; }

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
    Array<u8, kLength> m_address;
};

}
