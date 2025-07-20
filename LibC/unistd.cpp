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
    syscall(SYS_exit, status);
}

pid_t fork()
{
    pid_t ret = syscall(SYS_fork);
    RETURN_ERRNO(ret, ret, -1);
}

int execve(const char* pathname, char* const* argv)
{
    int ret = syscall(SYS_execve, (int)pathname, (int)argv);
    RETURN_ERRNO(ret, ret, -1);
}

int execvp(const char* file, char* const* argv)
{

    int ret = syscall(SYS_execve, (int)file, (int)argv);
    RETURN_ERRNO(ret, ret, -1);
}

int isatty(int fd)
{
    int ret = syscall(SYS_isatty, fd);
    RETURN_ERRNO(ret, ret, -1);
}

ssize_t write(int fd, const void* buf, size_t count)
{
    int ret = syscall(SYS_write, fd, (int)buf, count);
    RETURN_ERRNO(ret, ret, -1);
}

ssize_t read(int fd, void* buf, size_t count)
{
    int ret = syscall(SYS_read, fd, (int)buf, count);
    RETURN_ERRNO(ret, ret, -1);
}

pid_t getpid()
{
    return syscall(SYS_getpid);
}

pid_t getppid()
{
    return syscall(SYS_getppid);
}

uid_t getuid()
{
    return syscall(SYS_getuid);
}

char* getcwd(char* buf, size_t size)
{
    // TODO: POSIX.1-2001 states buf should be allocated when null
    if (buf == nullptr) {
        errno = EINVAL;
        return nullptr;
    }

    if (size == 0 && buf != nullptr) {
        errno = EINVAL;
        return nullptr;
    }

    int ret = syscall(SYS_getcwd, (int)buf, (int)size);
    RETURN_ERRNO(ret, buf, nullptr);
}

int chdir(const char* path)
{
    int ret = syscall(SYS_chdir, (int)path);
    RETURN_ERRNO(ret, ret, -1);
}

__END_DECLS
