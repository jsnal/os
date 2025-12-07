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

enum class IPv4Protocol : u8 {
    ICMP = 0x01,
    TCP = 0x06,
    UDP = 0x11
};

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
    void set_identification(u16 i) { m_identification = i; }

    u8 is_df_bit() const { return (m_flags_and_fragment_offset.host_value() & 0x4000) >> 14; }
    void set_df_bit(bool set)
    {
        if (set) {
            m_flags_and_fragment_offset = m_flags_and_fragment_offset.host_value() | 0x4000;
        } else {
            m_flags_and_fragment_offset = m_flags_and_fragment_offset.host_value() & ~0x4000;
        }
    }

    u8 is_mf_bit() const { return (m_flags_and_fragment_offset.host_value() & 0x2000) >> 13; }
    void set_mf_bit(bool set)
    {
        if (set) {
            m_flags_and_fragment_offset = m_flags_and_fragment_offset.host_value() | 0x2000;
        } else {
            m_flags_and_fragment_offset = m_flags_and_fragment_offset.host_value() & ~0x2000;
        }
    }

    u16 fragment_offset() const { return m_flags_and_fragment_offset.host_value() & 0x1FFF; }
    void set_fragment_offset(u16 o) { m_flags_and_fragment_offset = (m_flags_and_fragment_offset.host_value() & 0xE000) | (o & 0x1FFF); }

    u8 ttl() const { return m_ttl; }
    void set_ttl(u8 t) { m_ttl = t; }

    IPv4Protocol protocol() const { return static_cast<IPv4Protocol>(m_protocol); }
    void set_protocol(IPv4Protocol p) { m_protocol = static_cast<u8>(p); }

    u16 checksum() const { return m_checksum.host_value(); }
    void set_checksum(u16 c) { m_checksum = c; }

    IPv4Address source() const { return m_source; }
    void set_source(IPv4Address a) { m_source = a; }

    IPv4Address destination() const { return m_destination; }
    void set_destination(IPv4Address a) { m_destination = a; }

    const void* options() const
    {
        if (ihl() <= 5) {
            return nullptr;
        }
        return &m_data_and_options[0];
    }
    void* options()
    {
        if (ihl() <= 5) {
            return nullptr;
        }
        return &m_data_and_options[0];
    }

    const void* data() const
    {
        // TODO: Data will be after the options when IHL > 5
        ASSERT(ihl() == 5);
        return &m_data_and_options[0];
    }
    void* data()
    {
        ASSERT(ihl() == 5);
        return &m_data_and_options[0];
    }

private:
    u8 m_version_and_ihl { 0 }; // Version = high 4-bits, IHL = low 4-bits
    u8 m_dscp_and_enc { 0 };    // DSCP = high 6-bits, ENC = low 2-bits
    NetworkEndianness<u16> m_total_length { 0 };
    NetworkEndianness<u16> m_identification { 0 };
    NetworkEndianness<u16> m_flags_and_fragment_offset { 0 }; // Flags = high 3-bits, Fragment offset = low 13-bits
    u8 m_ttl { 0 };
    u8 m_protocol { 0 };
    NetworkEndianness<u16> m_checksum { 0 };
    IPv4Address m_source;
    IPv4Address m_destination;
    u8 m_data_and_options[0]; // Options exist if IHL is greater than 5
};

static_assert(sizeof(IPv4Packet) == 20);

inline u16 calculate_checksum(const void* buffer, size_t size)
{
    u32 sum = 0;
    const u16* ptr = reinterpret_cast<const u16*>(buffer);

    while (size > 1) {
        sum += convert_between_host_and_network(*ptr++);
        size -= 2;
    }

    if (size > 0) {
        sum += *reinterpret_cast<const u8*>(ptr);
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return static_cast<u16>(~sum);
}

}
