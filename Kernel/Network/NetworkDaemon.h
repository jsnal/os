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
    static void start();

private:
    void run();

    void handle_arp(E1000NetworkCard&, const EthernetHeader&, size_t size);
};

}
