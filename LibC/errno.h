/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _ERRNO_H_
#define _ERRNO_H_

#include <errno_defines.h>
#include <sys/cdefs.h>

#define RETURN_ERRNO(syscall_return, success_return, fail_return) \
    do {                                                          \
        if (syscall_return < 0) {                                 \
            errno = -syscall_return;                              \
            return fail_return;                                   \
        }                                                         \
        errno = 0;                                                \
        return success_return;                                    \
    } while (0)

__BEGIN_DECLS

extern int __errno;
#define errno (__errno)

__END_DECLS

#endif
