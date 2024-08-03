/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Logger.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Syscall.h>
#include <LibC/sys/syscall_defines.h>

namespace Syscall {

int handle(int call, int arg1, int arg2, int arg3)
{
    Process& p = PM.current_process();

    switch (call) {
        case SYS_EXIT:
            p.sys_exit(arg1);
            PM.yield();
            return 0;
        case SYS_WRITE:
            return p.sys_write(arg1, (const void*)arg2, arg3);
        case SYS_GETUID:
            return p.sys_getuid();
        default:
            dbgprintf_if(DEBUG_PROCESS, "Syscall", "Unknown syscall %u\n", call);
            break;
    }

    return 0;
}

void syscall_handler(InterruptFrame& frame)
{
    frame.eax = handle(frame.eax, frame.ebx, frame.ecx, frame.edx);
}

}
