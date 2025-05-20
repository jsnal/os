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

namespace Network {

void NetworkDaemon::start()
{
    NetworkDaemon daemon;
    daemon.run();
}

void NetworkDaemon::run()
{
    dbgprintln("NetworkDaemon", "Starting the Network Daemon");

    auto card = E1000NetworkCard::detect();

    while (true) {
        volatile bool empty = card->rx_queue().is_empty();
        if (empty) {
            PM.yield();
            continue;
        }

        auto buffer = card->rx_queue().dequeue();
        EthernetHeader* header = (EthernetHeader*)buffer.data();

        switch (header->type()) {
            case EthernetType::ARP:
                handle_arp(*card, *header, buffer.size());
                break;
            default:
                dbgprintln("NetworkDaemon", "Unhandled Ethernet frame: %d", header->type());
        }
    }
}

void NetworkDaemon::handle_arp(E1000NetworkCard& card, const EthernetHeader& header, size_t size)
{
    dbgprintln("NetworkDaemon", "Handling ARP from %s to %s",
        header.source().to_string().data(), header.destination().to_string().data());

    if (size < sizeof(EthernetHeader) + sizeof(ARPPacket)) {
        dbgprintln("NetworkDaemon", "Ethernet frame is too small for an ARP packet");
        return;
    }

    const ARPPacket& packet = *static_cast<const ARPPacket*>(header.data());

    size_t buffer_size = sizeof(EthernetHeader) + sizeof(ARPPacket);
    u8 buffer[buffer_size];

    EthernetHeader* eth_header = (EthernetHeader*)buffer;
    eth_header->set_destination(header.source());
    eth_header->set_source(card.mac_address());
    eth_header->set_type(EthernetType::ARP);

    ARPPacket arp_request;
    arp_request.set_hardware_type(ARPHardwareType::Ethernet);
    arp_request.set_protocol_type(ARPProtocolType::IPv4);
    arp_request.set_operation(ARPOperation::Response);
    arp_request.set_source_hardware_address(card.mac_address());
    arp_request.set_destination_hardware_address(packet.source_hardware_address());
    arp_request.set_source_protocol_address(packet.destination_protocol_address());
    arp_request.set_destination_protocol_address(packet.source_protocol_address());

    memcpy(buffer + sizeof(EthernetHeader), &arp_request, sizeof(ARPPacket));

    card.send(buffer, buffer_size);
}

}
