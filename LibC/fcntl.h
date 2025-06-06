/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <sys/types.h>

#define O_RDONLY 0x000000
#define O_WRONLY 0x000001
#define O_RDWR 0x000002
#define O_APPEND 0x000008
#define O_CREAT 0x000200
#define O_TRUNC 0x000400
#define O_EXCL 0x000800
#define O_SYNC 0x002000
#define O_NONBLOCK 0x004000
#define O_NOCTTY 0x008000
#define O_CLOEXEC 0x040000
#define O_NOFOLLOW 0x100000
#define O_DIRECTORY 0x200000
#define O_EXEC 0x400000
#define O_SEARCH O_EXEC

int open(const char*, int);
int open(const char*, int, mode_t);
