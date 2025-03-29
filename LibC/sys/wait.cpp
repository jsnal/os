/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <sys/syscall.h>
#include <sys/wait.h>

pid_t wait(int* wstatus)
{
    return waitpid(-1, wstatus, 0);
}

pid_t waitpid(pid_t pid, int* wstatus, int options)
{
    return syscall(SYS_waitpid, (int)pid, (int)wstatus, options);
}
