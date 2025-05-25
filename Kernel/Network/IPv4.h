/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Network/NetworkEndianness.h>
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

static_assert(sizeof(IPv4Address) == 4);

class [[gnu::packed]] IPv4Packet {
public:
    u8 version() const { return (m_version_and_ihl >> 4) & 0x0F; }
    void set_version(u8 v) { m_version_and_ihl = (m_version_and_ihl & 0x0F) | (v << 4); }

    u8 ihl() const { return m_version_and_ihl & 0x0F; }
    void set_ihl(u8 i) { m_version_and_ihl = (m_version_and_ihl & 0xF0) | (i & 0x0F); }

    u8 dscp() const { return (m_dscp_and_enc >> 2) & 0x3F; }
    void set_dscp(u8 d) { m_dscp_and_enc = (m_dscp_and_enc & 0x03) | (d << 2); }

    u8 ecn() const { return m_dscp_and_enc & 0x03; }
    void set_ecn(u8 e) { m_dscp_and_enc = (m_dscp_and_enc & 0xFC) | (e & 0x03); }

    u16 total_length() const { return m_total_length.host_value(); }
    void set_total_length(u16 l) { m_total_length = l; }

    u16 identification() const { return m_identification.host_value(); }
    void identification(u16 i) { m_identification = i; }

private:
    u8 m_version_and_ihl; // Version = high 4-bits, IHL = low 4-bits
    u8 m_dscp_and_enc;    // DSCP = high 6-bits, ENC = low 2-bits
    NetworkEndianness<u16> m_total_length;
    NetworkEndianness<u16> m_identification;
    NetworkEndianness<u16> m_flags_and_fragment_offset; // Flags = high 3-bits, Fragment offset = low 13-bits
    u8 m_ttl;
    u8 m_protocol;
    NetworkEndianness<u16> m_checksum;
    IPv4Address m_source;
    IPv4Address m_destination;
    u8 m_options[0];
};

static_assert(sizeof(IPv4Packet) == 20);

}
