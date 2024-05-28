/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <sys/syscall.h>

static int __attribute__((noinline)) __invoke_syscall_trap(int call, int arg1, int arg2, int arg3)
{
    int ret;
    asm volatile("int 0x80"
                 :
                 : "a"(call), "b"(arg1), "c"(arg2), "d"(arg3));

    asm volatile("mov %0, eax"
                 : "=r"(ret));

    return ret;
}

static inline int __attribute((always_inline)) __invoke_syscall(int call, int arg1, int arg2, int arg3, bool set_errno = true)
{
    int ret = __invoke_syscall_trap(call, arg1, arg2, arg3);
    if (set_errno && ret < 0) {
        errno = -ret;
        return -1;
    }
    return ret;
}

int syscall(int call)
{
    return __invoke_syscall(call, 0, 0, 0);
}

int syscall(int call, int arg1)
{
    return __invoke_syscall(call, 0, 0, 0);
}

int syscall(int call, int arg1, int arg2)
{
    return __invoke_syscall(call, arg1, arg2, 0);
}

int syscall(int call, int arg1, int arg2, int arg3)
{
    return __invoke_syscall(call, arg1, arg2, arg3);
}

int syscall_no_errno(int call)
{
    return __invoke_syscall(call, 0, 0, 0, false);
}
