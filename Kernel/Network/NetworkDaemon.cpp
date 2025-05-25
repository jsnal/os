/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Network/ARP.h>
#include <Kernel/Network/E1000NetworkCard.h>
#include <Kernel/Network/Ethernet.h>
#include <Kernel/Network/NetworkDaemon.h>
#include <Kernel/Process/ProcessManager.h>
#include <Universal/Logger.h>
#include <Universal/Stdlib.h>

#define DEBUG_NETWORK_DAEMON (0)

namespace Network {

NetworkDaemon& NetworkDaemon::the()
{
    static NetworkDaemon s_the;
    return s_the;
}

bool NetworkDaemon::detect()
{
    m_card = E1000NetworkCard::detect();
    return m_card.ptr() != nullptr;
}

void NetworkDaemon::run()
{
    dbgprintln("NetworkDaemon", "Starting the Network Daemon");

    while (true) {
        ASSERT(m_card.ptr() != nullptr);

        volatile bool empty = m_card->rx_queue().is_empty();
        if (empty) {
            PM.yield();
            continue;
        }

        auto buffer = m_card->rx_queue().dequeue();
        EthernetHeader* header = (EthernetHeader*)buffer.data();

        switch (header->type()) {
            case EthernetType::ARP:
                handle_arp(*header, buffer.size());
                break;
            case EthernetType::IPv4:
                handle_ipv4(*header, buffer.size());
                break;
            case EthernetType::IPv6:
                dbgprintln_if(DEBUG_NETWORK_DAEMON, "NetworkDaemon", "Found IPv6 packet");
                break;
            default:
                dbgprintln("NetworkDaemon", "Unhandled Ethernet frame: %#02x", static_cast<u16>(header->type()));
        }
    }
}

void NetworkDaemon::handle_arp(const EthernetHeader& header, size_t size)
{
    dbgprintln("NetworkDaemon", "Handling ARP from %s to %s",
        header.source().to_string().data(), header.destination().to_string().data());

    if (size < sizeof(EthernetHeader) + sizeof(ARPPacket)) {
        dbgprintln("NetworkDaemon", "Ethernet frame is too small for an ARP packet");
        return;
    }

    const ARPPacket& rx_packet = *static_cast<const ARPPacket*>(header.data());

    ARPPacket tx_packet;
    tx_packet.set_hardware_type(ARPHardwareType::Ethernet);
    tx_packet.set_protocol_type(ARPProtocolType::IPv4);
    tx_packet.set_operation(ARPOperation::Response);
    tx_packet.set_source_hardware_address(m_card->mac_address());
    tx_packet.set_destination_hardware_address(rx_packet.source_hardware_address());
    tx_packet.set_source_protocol_address(m_card->ipv4_address());
    tx_packet.set_destination_protocol_address(rx_packet.source_protocol_address());

    m_card->send(header.source(), tx_packet);
}

void NetworkDaemon::handle_ipv4(const EthernetHeader& header, size_t size)
{
    dbgprintln("NetworkDaemon", "Handling IPv4 packet");

    const IPv4Packet& rx_packet = *static_cast<const IPv4Packet*>(header.data());

    dbgprintln("NetworkDaemon", "version = %x", rx_packet.version());
    dbgprintln("NetworkDaemon", "ihl = %x", rx_packet.ihl());
    dbgprintln("NetworkDaemon", "dscp = %x", rx_packet.dscp());
    dbgprintln("NetworkDaemon", "ecn = %x", rx_packet.ecn());
    dbgprintln("NetworkDaemon", "total length = %u", rx_packet.total_length());
    dbgprintln("NetworkDaemon", "identification = %x", rx_packet.identification());
}

}
