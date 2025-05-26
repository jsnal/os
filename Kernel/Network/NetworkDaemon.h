/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Network/E1000NetworkCard.h>
#include <Kernel/Network/Ethernet.h>

namespace Network {

class NetworkDaemon {
public:
    static NetworkDaemon& the();
    static void start() { the().run(); };

    bool detect();

private:
    void run();

    void handle_arp(const EthernetHeader&, size_t size);
    void handle_ipv4(const EthernetHeader&, size_t size);
    void handle_icmp(const EthernetHeader&, const IPv4Packet&);

    UniquePtr<E1000NetworkCard> m_card;
};

}
