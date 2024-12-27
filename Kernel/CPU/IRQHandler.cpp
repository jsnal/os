/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/CPU/IRQHandler.h>
#include <Kernel/CPU/PIC.h>
#include <Kernel/Logger.h>

IRQHandler* s_handlers[IRQHandler::handler_count];

IRQHandler::IRQHandler(IRQ irq)
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

Result IRQHandler::set_irq(u8 irq)
{
    if (irq < 0 || irq > 16 || s_handlers[irq] != nullptr) {
        return Result::Failure;
    }

    if (m_irq != -1) {
        PIC::mask(m_irq);
        s_handlers[m_irq] = nullptr;
    }

    disable_irq();
    m_irq = static_cast<IRQ>(irq);
    s_handlers[m_irq] = this;

    return Result::OK;
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

    if (handler->irq_enabled()) {
        handler->handle_irq(frame);
    }

    handler->send_eoi();
}

void IRQHandler::send_eoi()
{
    if (!m_eoi_sent) {
        PIC::eoi(m_irq);
        m_eoi_sent = true;
    }
}
