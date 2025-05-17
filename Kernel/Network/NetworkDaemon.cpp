/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Network/ARP.h>
#include <Kernel/Network/E1000NetworkCard.h>
#include <Kernel/Network/Ethernet.h>
#include <Kernel/Network/NetworkDaemon.h>
#include <Universal/Logger.h>
#include <Universal/Stdlib.h>

namespace Network {

void NetworkDaemon::run()
{
    dbgprintln("NetworkDaemon", "Starting the Network Daemon");

    auto card = E1000NetworkCard::detect();

    size_t buffer_size = sizeof(EthernetHeader) + sizeof(ARPPacket);
    u8 buffer[buffer_size];

    EthernetHeader* eth_header = (EthernetHeader*)buffer;
    eth_header->set_destination({});
    eth_header->set_source(card->mac_address());
    eth_header->set_type(EthernetType::ARP);

    ARPPacket arp_request;
    arp_request.set_hardware_type(ARPHardwareType::Ethernet);
    arp_request.set_protocol_type(ARPProtocolType::IPv4);
    arp_request.set_operation(ARPOperation::Request);
    arp_request.set_source_hardware_address(card->mac_address());
    arp_request.set_destination_hardware_address({ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff });

    // TODO this should be a real structure
    u8 destination_protocol_address[4] = { 192, 168, 1, 124 };
    u8 source_protocol_address[4] = { 10, 0, 2, 5 };
    arp_request.set_source_protocol_address(source_protocol_address);
    arp_request.set_destination_protocol_address(destination_protocol_address);

    memcpy(buffer + sizeof(EthernetHeader), &arp_request, sizeof(ARPPacket));

    card->send(buffer, buffer_size);

    while (true) {
    }
}
}
