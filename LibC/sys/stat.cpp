/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <sys/stat.h>
#include <sys/syscall.h>

int fstat(int fd, struct stat* statbuf)
{
    int ret = syscall(SYS_fstat, fd, (int)statbuf);
    RETURN_ERRNO(ret, ret, -1);
}
