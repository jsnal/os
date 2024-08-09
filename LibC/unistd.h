/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <sys/cdefs.h>
#include <sys/types.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

__BEGIN_DECLS

void _exit(int status);

int isatty(int fd);

ssize_t write(int fd, const void* buf, size_t count);
ssize_t read(int fd, void* buf, size_t count);

uid_t get_uid();

__END_DECLS

#endif
