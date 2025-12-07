/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Network/NetworkEndianness.h>
#include <Universal/Types.h>

namespace Network {

enum class ICMPType : u8 {
    EchoReply = 0x00,
    EchoRequest = 0x08,
};

class [[gnu::packed]] ICMPEchoData {
public:
    u16 identifier() const { return m_identifier.host_value(); }
    void set_identifier(u16 i) { m_identifier = i; }

    u16 sequence_number() const { return m_sequence_number.host_value(); }
    void set_sequence_number(u16 s) { m_sequence_number = s; }

    const void* data() const { return m_data; }
    void* data() { return m_data; }

private:
    NetworkEndianness<u16> m_identifier;
    NetworkEndianness<u16> m_sequence_number;
    u8 m_data[0];
};

static_assert(sizeof(ICMPEchoData) == 4);

class [[gnu::packed]] ICMPPacket {
public:
    ICMPType type() const { return static_cast<ICMPType>(m_type); }
    void set_type(ICMPType t) { m_type = static_cast<u8>(t); }

    u8 code() const { return m_code; }
    void set_code(u8 c) { m_code = c; }

    u16 checksum() const { return m_checksum.host_value(); }
    void set_checksum(u16 c) { m_checksum = c; }

    //    template<typename T>
    //    const T* data() const { return reinterpret_cast<const T*>(m_data); }
    //    template<typename T>
    //    T* data() { return reinterpret_cast<T*>(m_data); }
    const void* data() const { return m_data; }
    void* data() { return m_data; }

private:
    u8 m_type;
    u8 m_code;
    NetworkEndianness<u16> m_checksum;

    // Rest of the header is 4-bytes long where the content depends on
    // the type of the ICMP packet
    u8 m_data[0];
};

static_assert(sizeof(ICMPPacket) == 4);

}
