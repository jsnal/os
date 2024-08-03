/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <sys/syscall.h>
#include <sys/syscall_defines.h>
#include <unistd.h>

void _exit(int status)
{
    syscall(SYS_EXIT, status);
}

ssize_t write(int fd, const void* buf, size_t count)
{
    return syscall(SYS_WRITE, fd, (int)buf, count);
}

uid_t get_uid(void)
{
    return syscall_no_errno(SYS_GETUID);
}
