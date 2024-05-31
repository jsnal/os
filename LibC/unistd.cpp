/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/API/syscalls.h>
#include <sys/syscall.h>
#include <unistd.h>

void _exit(int status)
{
    syscall(SYS_EXIT, status);
}

uid_t get_uid(void)
{
    return syscall_no_errno(SYS_GETUID);
}
