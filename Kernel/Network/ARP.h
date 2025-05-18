/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Network/IPv4.h>
#include <Kernel/Network/MACAddress.h>
#include <Kernel/Network/NetworkEndianness.h>
#include <Universal/Types.h>

namespace Network {

enum class ARPHardwareType : u16 {
    Ethernet = 0x1,
};

enum class ARPProtocolType : u16 {
    IPv4 = 0x0800,
};

enum class ARPOperation : u16 {
    Request = 0x1,
    Response = 0x2,
};

class [[gnu::packed]] ARPPacket {
public:
    ARPHardwareType hardware_type() const { return static_cast<ARPHardwareType>(m_hardware_type.host_value()); }
    void set_hardware_type(ARPHardwareType type) { m_hardware_type = static_cast<u16>(type); }

    ARPProtocolType protocol_type() const { return static_cast<ARPProtocolType>(m_protocol_type.host_value()); }
    void set_protocol_type(ARPProtocolType type) { m_protocol_type = static_cast<u16>(type); }

    u8 hardware_address_length() const { return m_hardware_address_length; }
    u8 protocol_address_length() const { return m_protocol_address_length; }

    ARPOperation operation() const { return static_cast<ARPOperation>(m_operation.host_value()); }
    void set_operation(ARPOperation o) { m_operation = static_cast<u16>(o); }

    MACAddress source_hardware_address() const { return m_source_hardware_address; }
    void set_source_hardware_address(MACAddress a) { m_source_hardware_address = a; }

    IPv4Address source_protocol_address(IPv4Address a) { return m_source_protocol_address; }
    void set_source_protocol_address(IPv4Address a) { m_source_protocol_address = a; }

    IPv4Address destination_protocol_address(IPv4Address a) { return m_destination_protocol_address; }
    void set_destination_protocol_address(IPv4Address a) { m_destination_protocol_address = a; }

    MACAddress destination_hardware_address() const { return m_destination_hardware_address; }
    void set_destination_hardware_address(MACAddress a) { m_destination_hardware_address = a; }

private:
    NetworkEndianness<u16> m_hardware_type;
    NetworkEndianness<u16> m_protocol_type;
    u8 m_hardware_address_length { MACAddress::kLength };
    u8 m_protocol_address_length { IPv4Address::kLength };
    NetworkEndianness<u16> m_operation;
    MACAddress m_source_hardware_address;
    IPv4Address m_source_protocol_address;
    MACAddress m_destination_hardware_address;
    IPv4Address m_destination_protocol_address;
};

}
