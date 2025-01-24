/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <sys/syscall_defines.h>

int syscall(SyscallOpcode call);
int syscall(SyscallOpcode call, int arg1);
int syscall(SyscallOpcode call, int arg1, int arg2);
int syscall(SyscallOpcode call, int arg1, int arg2, int arg3);

#endif
