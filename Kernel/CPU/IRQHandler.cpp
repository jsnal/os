/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/CPU/IRQHandler.h>

#define IRQ_COUNT 16

IRQHandler* s_handlers[IRQ_COUNT];

IRQHandler::IRQHandler(int irq)
    : m_irq(irq)
{
    s_handlers[m_irq] = this;
    IDT::register_interrupt_handler(m_irq, handle_all_irqs);
}

IRQHandler::~IRQHandler()
{
    s_handlers[m_irq] = nullptr;
    IDT::unregister_interrupt_handler(m_irq);
}

void IRQHandler::handle_all_irqs(InterruptFrame* frame)
{
    IRQHandler* handler = s_handlers[frame->interrupt_number - 0x20];
    if (handler == nullptr || !handler->enabled()) {
        return;
    }

    s_handlers[frame->interrupt_number]->handle();
}

void IRQHandler::enable_irq()
{
    m_enabled = true;
}

void IRQHandler::disable_irq()
{
    m_enabled = false;
}
