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

#define DEBUG_NETWORK_DAEMON (1)

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
}

void NetworkDaemon::handle_icmp(const EthernetHeader& header, const IPv4Packet& ipv4_packet)
{
    const ICMPPacket& icmp_packet = *static_cast<const ICMPPacket*>(ipv4_packet.data());
    size_t packet_size = ipv4_packet.total_length() - sizeof(IPv4Packet);

    if (calculate_checksum(&icmp_packet, packet_size) != 0) {
        dbgprintln("NetworkDaemon", "Bad ICMP checksum... dropping");
        return;
    }

    switch (icmp_packet.type()) {
        case ICMPType::EchoRequest: {
            const ICMPEchoData& icmp_echo_request_data = *reinterpret_cast<const ICMPEchoData*>(icmp_packet.data());

            dbgprintln_if(true, "NetworkDaemon", "Handling echo request from %s: id=%u, seq=%u",
                ipv4_packet.source().to_string().data(), icmp_echo_request_data.identifier(), icmp_echo_request_data.sequence_number());

            u8 buffer[packet_size];

            ICMPPacket& icmp_packet = *reinterpret_cast<ICMPPacket*>(buffer);
            icmp_packet.set_type(ICMPType::EchoReply);
            icmp_packet.set_code(0);
            icmp_packet.set_checksum(0);

            ICMPEchoData& icmp_echo_response_data = *reinterpret_cast<ICMPEchoData*>(icmp_packet.data());
            icmp_echo_response_data.set_identifier(icmp_echo_request_data.identifier());
            icmp_echo_response_data.set_sequence_number(icmp_echo_request_data.sequence_number());

            size_t left_over_size = packet_size - (sizeof(ICMPPacket) + sizeof(ICMPEchoData));
            if (left_over_size > 0) {
                memcpy(icmp_echo_response_data.data(), icmp_echo_request_data.data(), left_over_size);
            }

            icmp_packet.set_checksum(calculate_checksum(buffer, packet_size));
            m_card->send(header.source(), ipv4_packet.source(), buffer, packet_size);
            break;
        }
        default:
            dbgprintln("NetworkDaemon", "Unhandled ICMP type: %#01x", static_cast<u8>(icmp_packet.type()));
            break;
    }
}

}
