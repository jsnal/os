/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <fcntl.h>
#include <sys/syscall.h>

int open(const char* pathname, int flags)
{
    return syscall(SYS_open, (int)pathname, flags, 0);
}

int open(const char* pathname, int flags, mode_t mode)
{
    return syscall(SYS_open, (int)pathname, flags, mode);
}
