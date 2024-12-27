/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/CPU/IDT.h>
#include <Universal/Types.h>

#define IRQ_HANDLER_COUNT 16

#define IRQ_PIT 0
#define IRQ_KEYBOARD 1
#define IRQ_DISK_PRIMARY 14
#define IRQ_DISK_SECONDARY 15

class IRQHandler {
public:
    static void handle_all_irqs(const InterruptFrame&);

protected:
    IRQHandler() = default;
    IRQHandler(u8 irq);
    ~IRQHandler();

    void set_irq(u8 irq);

    void send_eoi();

    bool irq_enabled() const { return m_enabled; }
    void enable_irq() { m_enabled = true; };
    void disable_irq() { m_enabled = false; };

private:
    virtual void handle_irq(const InterruptFrame&) = 0;

    // Use -1 as a sentinel value since 0 is a valid IRQ
    i16 m_irq { -1 };

    bool m_enabled { false };
    bool m_eoi_sent { false };
};
