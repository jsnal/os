/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#ifdef KERNEL
#    include <Kernel/Assert.h>
#else
#    define ASSERT(fmt, ...) \
        do {                 \
        } while (0)
#endif
