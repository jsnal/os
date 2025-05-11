/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/syscall.h>

int open(const char* pathname, int flags)
{
    int ret = syscall(SYS_open, (int)pathname, flags, 0);
    RETURN_ERRNO(ret, ret, -1);
}

int open(const char* pathname, int flags, mode_t mode)
{
    int ret = syscall(SYS_open, (int)pathname, flags, mode);
    RETURN_ERRNO(ret, ret, -1);
}
