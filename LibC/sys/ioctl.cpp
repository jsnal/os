/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>

int ioctl(int fd, unsigned long request, ...)
{
    va_list ap;
    va_start(ap, request);
    unsigned arg = va_arg(ap, unsigned);
    int ret = syscall(SYS_ioctl, fd, (int)request, (int)arg);
    va_end(ap);
    RETURN_ERRNO(ret, ret, -1);
}
