/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/CPU/IRQHandler.h>
#include <Kernel/CPU/PIC.h>
#include <Kernel/Logger.h>

IRQHandler* s_handlers[IRQ_HANDLER_COUNT];

IRQHandler::IRQHandler(int irq)
    : m_irq(irq)
{
    s_handlers[m_irq] = this;
    PIC::unmask(m_irq);
}

IRQHandler::~IRQHandler()
{
    PIC::mask(m_irq);
    s_handlers[m_irq] = nullptr;
}

void IRQHandler::handle_all_irqs(const InterruptFrame& frame)
{
    IRQHandler* handler = s_handlers[frame.interrupt_number - 32];
    ASSERT(handler != nullptr);

    handler->m_eoi_sent = false;

    if (handler->enabled()) {
        handler->handle();
    }

    handler->eoi();
}
void IRQHandler::eoi()
{
    if (!m_eoi_sent) {
        PIC::eoi(m_irq);
        m_eoi_sent = true;
    }
}

void IRQHandler::enable_irq()
{
    m_enabled = true;
}

void IRQHandler::disable_irq()
{
    m_enabled = false;
}
