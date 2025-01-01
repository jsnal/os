/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define SYSCALL_OPCODE_LIST \
    SYSCALL_OPCODE(exit)    \
    SYSCALL_OPCODE(fork)    \
    SYSCALL_OPCODE(execve)  \
    SYSCALL_OPCODE(write)   \
    SYSCALL_OPCODE(read)    \
    SYSCALL_OPCODE(getpid)  \
    SYSCALL_OPCODE(getuid)  \
    SYSCALL_OPCODE(ioctl)   \
    SYSCALL_OPCODE(isatty)

#define SYSCALL_OPCODE(x) SYS_##x,
enum SyscallOpcode {
    SYSCALL_OPCODE_LIST
};
#undef SYSCALL_OPCODE

#define SYSCALL_OPCODE(x) \
    case SYS_##x:         \
        return #x;
inline constexpr const char* syscall_opcode_to_string(SyscallOpcode call)
{
    switch (call) {
        SYSCALL_OPCODE_LIST
        default:
            break;
    }

    return "Unknown ";
}
#undef SYSCALL_OPCODE
