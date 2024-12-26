/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/CPU/IRQHandler.h>
#include <Kernel/CPU/PIC.h>
#include <Kernel/Logger.h>

IRQHandler* s_handlers[IRQ_HANDLER_COUNT];

IRQHandler::IRQHandler(u8 irq)
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

void IRQHandler::set_irq(u8 irq)
{
    if (m_irq != 0xFF) {
        PIC::mask(m_irq);
        s_handlers[m_irq] = nullptr;
    }

    disable_irq();
    m_irq = irq;
    s_handlers[m_irq] = this;
}

void IRQHandler::handle_all_irqs(const InterruptFrame& frame)
{
    u8 irq_number = frame.interrupt_number - 32;
    IRQHandler* handler = s_handlers[irq_number];

    if (handler == nullptr) {
        if (irq_number == 7) {
            u8 isr = PIC::read_isr();
            if (!(isr & (1 << 7))) {
                return;
            }
        }

        panic("Unhanled IRQ %u\n", irq_number);
    }

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
