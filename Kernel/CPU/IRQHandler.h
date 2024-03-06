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
    virtual void handle() = 0;

protected:
    IRQHandler() {};

    IRQHandler(int irq);
    ~IRQHandler();

    static void handle_all_irqs(InterruptFrame*);

    bool enabled() const { return m_enabled; }

    void enable_irq();
    void disable_irq();

private:
    u8 m_irq { 0 };

    bool m_enabled { false };
};
