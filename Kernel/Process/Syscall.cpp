/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Process/Process.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Syscall.h>
#include <LibC/sys/syscall_defines.h>
#include <Universal/Logger.h>

#define TRACE_SYSCALLS 0

namespace Syscall {

int handle(TaskRegisters& regs, SyscallOpcode call, int arg1, int arg2, int arg3)
{
    Process& p = PM.current_process();

    dbgprintf_if(TRACE_SYSCALLS, "Syscall", "%s called %s()\n", p.name().data(), syscall_opcode_to_string(call));

    switch (call) {
        case SYS_exit:
            p.sys_exit(arg1);
            PM.yield();
            return 0;
        case SYS_fork:
            return p.sys_fork(regs);
        case SYS_waitpid:
            return p.sys_waitpid((pid_t)arg1, (int*)arg2, arg3);
        case SYS_execve:
            return p.sys_execve((const char*)arg1, (char* const*)arg2);
        case SYS_open:
            return p.sys_open((const char*)arg1, arg2, (mode_t)arg3);
        case SYS_write:
            return p.sys_write(arg1, (const void*)arg2, arg3);
        case SYS_read:
            return p.sys_read(arg1, (void*)arg2, arg3);
        case SYS_getpid:
            return p.sys_getpid();
        case SYS_getppid:
            return p.sys_getppid();
        case SYS_getuid:
            return p.sys_getuid();
        case SYS_ioctl:
            return p.sys_ioctl(arg1, arg2, (uint32_t*)arg3);
        case SYS_isatty:
            return p.sys_isatty(arg1);
        case SYS_mmap:
            return (int)p.sys_mmap((const mmap_args*)arg1);
        case SYS_munmap:
            return p.sys_munmap((void*)arg1, arg2);
        case SYS_dbgwrite:
            return p.sys_dbgwrite((const char*)arg1, arg2);
        default:
            dbgprintf_if(TRACE_SYSCALLS, "Syscall", "Unknown syscall %u\n", call);
            break;
    }

    return 0;
}

void syscall_handler(TaskRegisters& regs)
{
    regs.general_purpose.eax = handle(regs, static_cast<SyscallOpcode>(regs.general_purpose.eax),
        regs.general_purpose.ebx, regs.general_purpose.ecx, regs.general_purpose.edx);
}

}
