/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <sys/types.h>

#define SYSCALL_OPCODE_LIST       \
    SYSCALL_OPCODE(chdir)         \
    SYSCALL_OPCODE(dbgwrite)      \
    SYSCALL_OPCODE(execve)        \
    SYSCALL_OPCODE(exit)          \
    SYSCALL_OPCODE(fork)          \
    SYSCALL_OPCODE(fstat)         \
    SYSCALL_OPCODE(getcwd)        \
    SYSCALL_OPCODE(getdirentries) \
    SYSCALL_OPCODE(getpid)        \
    SYSCALL_OPCODE(getppid)       \
    SYSCALL_OPCODE(getuid)        \
    SYSCALL_OPCODE(ioctl)         \
    SYSCALL_OPCODE(isatty)        \
    SYSCALL_OPCODE(mmap)          \
    SYSCALL_OPCODE(munmap)        \
    SYSCALL_OPCODE(open)          \
    SYSCALL_OPCODE(read)          \
    SYSCALL_OPCODE(waitpid)       \
    SYSCALL_OPCODE(write)

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

struct mmap_args {
    void* addr;
    size_t length;
    int prot;
    int flags;
    int fd;
    off_t offset;
};
