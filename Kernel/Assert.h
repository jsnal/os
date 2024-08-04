/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/panic.h>

#define ASSERT(expr) (static_cast<bool>(expr) ? (void)0 : assert_failed(#expr, __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__))
#define ASSERT_INTERRUPTS_DISABLED() ASSERT(!(CPU::cpu_flags() & 0x200))
#define NO_IMPLEMENTATION() (ASSERT(false))

static inline void assert_failed(const char* message, const char* file, const char* function, u32 line)
{
    panic("Assert failed: %s \n%s:%d in %s\n", message, function, line, file);
}
