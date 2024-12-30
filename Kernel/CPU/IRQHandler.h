/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/CPU/IDT.h>
#include <Universal/Types.h>

class IRQHandler {
public:
    // Number of possible IRQs but all of them are not supported
    constexpr static u8 handler_count = 16;

    enum IRQ {
        UNKNOWN = -1,
        PIT = 0,
        KEYBOARD = 1,
        PERIPHERAL_1 = 9,
        PERIPHERAL_2 = 10,
        PERIPHERAL_3 = 11,
        DISK_PRIMARY = 14,
        DISK_SECONDARY = 15
    };

    static void handle_all_irqs(const InterruptRegisters&);

protected:
    IRQHandler() = default;
    IRQHandler(IRQ irq);
    ~IRQHandler();

    Result set_irq(u8 irq);

    void send_eoi();

    bool irq_enabled() const { return m_enabled; }
    void enable_irq() { m_enabled = true; };
    void disable_irq() { m_enabled = false; };

private:
    virtual void handle_irq(const InterruptRegisters&) = 0;

    IRQ m_irq { UNKNOWN };
    bool m_enabled { false };
    bool m_eoi_sent { false };
};
