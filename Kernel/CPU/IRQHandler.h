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
    IRQHandler() {};

    IRQHandler(u8 irq);
    ~IRQHandler();

    void set_irq(u8 irq);

    bool enabled() const { return m_enabled; }

    void eoi();

    void enable_irq();
    void disable_irq();

private:
    virtual void handle() = 0;

    u8 m_irq { 0xFF };

    bool m_enabled { false };
    bool m_eoi_sent { false };
};
