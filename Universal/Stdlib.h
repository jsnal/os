/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#ifdef KERNEL
#    include <Kernel/Stdlib.h>
#else
#    include <stdlib.h>
#    include <string.h>
#endif

template<typename T>
T&& move(T& arg)
{
    return static_cast<T&&>(arg);
}
