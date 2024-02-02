/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _STDARG_H_
#define _STDARG_H_

typedef unsigned char* va_list;

#define va_start(ap, last) \
    ((ap) = ((va_list) & (last)) + (sizeof(last)))

#define va_end(ap) \
    ((ap) = (va_list)0)

#define va_arg(ap, type) \
    (((ap) = (ap) + (sizeof(type))), *((type*)((ap) - (sizeof(type)))))

#endif
