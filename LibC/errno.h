/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/API/errno.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

extern int __errno;
#define errno (__errno)

__END_DECLS
