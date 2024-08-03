/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _IOCTL_H_
#define _IOCTL_H_

#include <sys/ioctl_defines.h>

int ioctl(int fd, unsigned long request, ...);

#endif
