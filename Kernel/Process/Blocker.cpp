/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Process/ProcessManager.h>
#include <Universal/Logger.h>
#include <Universal/Malloc.h>

bool WaitBlocker::is_ready()
{
    if (m_pid != -1) {
        dbgprintln("WaitBlocker", "Only pid -1 is supported");
        return true;
    }

    bool ready = false;
    PM.for_each_child(m_parent, [&](Process& child) {
        if (!child.is_dead()) {
            return true;
        }

        m_pid = child.pid();
        ready = true;
        return false;
    });

    return ready;
}
