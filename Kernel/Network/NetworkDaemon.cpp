/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Network/ARP.h>
#include <Kernel/Network/E1000NetworkCard.h>
#include <Kernel/Network/Ethernet.h>
#include <Kernel/Network/ICMP.h>
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
        const EthernetHeader& header = *reinterpret_cast<const EthernetHeader*>(buffer.data());

        switch (header.type()) {
            case EthernetType::ARP:
                handle_arp(header, buffer.size());
                break;
            case EthernetType::IPv4:
                handle_ipv4(header, buffer.size());
                break;
            case EthernetType::IPv6:
                dbgprintln_if(DEBUG_NETWORK_DAEMON, "NetworkDaemon", "Found IPv6 packet");
                break;
            default:
                dbgprintln("NetworkDaemon", "Unhandled Ethernet frame: %#02x", static_cast<u16>(header.type()));
        }
    }
}

void NetworkDaemon::handle_arp(const EthernetHeader& header, size_t size)
{
    dbgprintln_if(DEBUG_NETWORK_DAEMON, "NetworkDaemon", "Handling ARP from %s to %s",
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
    dbgprintln_if(DEBUG_NETWORK_DAEMON, "NetworkDaemon", "Handling IPv4 from %s to %s",
        header.source().to_string().data(), header.destination().to_string().data());

    if (size < sizeof(EthernetHeader) + sizeof(IPv4Packet)) {
        dbgprintln("NetworkDaemon", "Ethernet frame is too small for an IPv4 packet");
        return;
    }

    const IPv4Packet& rx_packet = *static_cast<const IPv4Packet*>(header.data());

    switch (rx_packet.protocol()) {
        case IPv4Protocol::ICMP:
            handle_icmp(header, rx_packet);
            break;
        default:
            break;
    }

    //    dbgprintln("NetworkDaemon", "version = %x", rx_packet.version());
    //    dbgprintln("NetworkDaemon", "ihl = %x", rx_packet.ihl());
    //    dbgprintln("NetworkDaemon", "dscp = %x", rx_packet.dscp());
    //    dbgprintln("NetworkDaemon", "ecn = %x", rx_packet.ecn());
    //    dbgprintln("NetworkDaemon", "total length = %u", rx_packet.total_length());
    //    dbgprintln("NetworkDaemon", "identification = %x", rx_packet.identification());
    //    dbgprintln("NetworkDaemon", "flags:");
    //    dbgprintln("NetworkDaemon", "  DF = %x", rx_packet.is_df_bit());
    //    dbgprintln("NetworkDaemon", "  MF = %x", rx_packet.is_mf_bit());
    //    dbgprintln("NetworkDaemon", "fragment offset = %u", rx_packet.fragment_offset());
}

void NetworkDaemon::handle_icmp(const EthernetHeader& header, const IPv4Packet& ipv4_packet)
{
    const ICMPPacket& icmp_packet = *static_cast<const ICMPPacket*>(ipv4_packet.data());

    switch (icmp_packet.type()) {
        case ICMPType::EchoRequest: {
            const ICMPEchoData& echo_data = *icmp_packet.data<ICMPEchoData>();

            dbgprintln_if(true, "NetworkDaemon", "Handling echo request from %s: id=%u, seq=%u",
                ipv4_packet.source().to_string().data(), echo_data.identifier(), echo_data.sequence_number());

            m_card->send(header.source(), ipv4_packet.source(), icmp_packet);
            break;
        }
        default:
            dbgprintln("NetworkDaemon", "Unhandled ICMP type: %#01x", static_cast<u8>(icmp_packet.type()));
            break;
    }
}

}
