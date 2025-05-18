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

#pragma GCC push_options
#pragma GCC optimize("-O0")
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
    arp_request.set_source_protocol_address({ 10, 0, 2, 5 });
    arp_request.set_destination_protocol_address({ 192, 168, 1, 124 });

    memcpy(buffer + sizeof(EthernetHeader), &arp_request, sizeof(ARPPacket));

    // card->send(buffer, buffer_size);

    while (true) {
        volatile bool empty = card->rx_queue().is_empty();
        if (empty) {
            PM.yield();
            continue;
        }

        // if (!empty) {
        //  PM.yield();
        auto b = card->rx_queue().dequeue();
        dbgprintln("NetworkDaemon", "Got the data from the network card!");

        EthernetHeader* header = (EthernetHeader*)b.ptr();
        // dbgprintln("NetworkDaemon", "From %s to %s", header->source(), header->destination());
        //}
    }
}
#pragma GCC pop_options

}
