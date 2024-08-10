/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <sys/syscall.h>
#include <sys/syscall_defines.h>
#include <unistd.h>

__BEGIN_DECLS

void _exit(int status)
{
    syscall(SYS_EXIT, status);
}

int isatty(int fd)
{
    int ret = syscall(SYS_ISATTY, fd);
    RETURN_ERRNO(ret, ret, -1);
}

ssize_t write(int fd, const void* buf, size_t count)
{
    int ret = syscall(SYS_WRITE, fd, (int)buf, count);
    RETURN_ERRNO(ret, ret, -1);
}

ssize_t read(int fd, void* buf, size_t count)
{
    int ret = syscall(SYS_READ, fd, (int)buf, count);
    RETURN_ERRNO(ret, ret, -1);
}

uid_t get_uid(void)
{
    return syscall_no_errno(SYS_GETUID);
}

__END_DECLS
