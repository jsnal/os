/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Network/MACAddress.h>
#include <Kernel/Network/NetworkEndianness.h>
#include <Universal/Types.h>

namespace Network {

enum class EthernetType : u16 {
    ARP = 0x0806,
};

class [[gnu::packed]] EthernetHeader {
public:
    MACAddress destination() const { return m_destination; }
    void set_destination(MACAddress d) { m_destination = d; }

    MACAddress source() const { return m_source; }
    void set_source(MACAddress s) { m_source = s; };

    EthernetType type() const { return static_cast<EthernetType>(m_type.host_value()); }
    void set_type(EthernetType t) { m_type = static_cast<u16>(t); }

    const void* data() const { return &m_data[0]; }
    void* data() { return &m_data[0]; }

private:
    MACAddress m_destination;
    MACAddress m_source;
    NetworkEndianness<u16> m_type;
    u32 m_data[0];
};

}
