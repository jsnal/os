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
    if (handler != nullptr && handler->enabled()) {
        handler->handle();
    }

    if (frame.interrupt_number == 46) {
        dbgprintf("IRQ", "Got disk drive interrupt 46\n");
    }
    if (frame.interrupt_number == 47) {
        dbgprintf("IRQ", "Got disk drive interrupt 47\n");
    }

    PIC::eoi(frame.interrupt_number - 32);
}

void IRQHandler::enable_irq()
{
    m_enabled = true;
}

void IRQHandler::disable_irq()
{
    m_enabled = false;
}
