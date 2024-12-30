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

int handle(SyscallRegisters& regs, int call, int arg1, int arg2, int arg3)
{
    Process& p = PM.current_process();

    switch (call) {
        case SYS_EXIT:
            p.sys_exit(arg1);
            PM.yield();
            return 0;
        case SYS_FORK:
            return p.sys_fork(regs);
        case SYS_WRITE:
            return p.sys_write(arg1, (const void*)arg2, arg3);
        case SYS_READ:
            return p.sys_read(arg1, (void*)arg2, arg3);
        case SYS_GETPID:
            return p.sys_getpid();
        case SYS_GETUID:
            return p.sys_getuid();
        case SYS_IOCTL:
            return p.sys_ioctl(arg1, arg2, (uint32_t*)arg3);
        case SYS_ISATTY:
            return p.sys_isatty(arg1);
        default:
            dbgprintf_if(DEBUG_PROCESS, "Syscall", "Unknown syscall %u\n", call);
            break;
    }

    return 0;
}

void syscall_handler(SyscallRegisters& regs)
{
    regs.general_purpose.eax = handle(regs, regs.general_purpose.eax, regs.general_purpose.ebx,
        regs.general_purpose.ecx, regs.general_purpose.edx);
}

}
