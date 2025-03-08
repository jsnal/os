/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/CPU/CPU.h>

#define ASSERT(expr) (static_cast<bool>(expr) ? (void)0 : CPU::assertion_failed(#expr, __FILE_NAME__, __FUNCTION__, __LINE__))
#define ASSERT_INTERRUPTS_DISABLED() ASSERT(!(CPU::cpu_flags() & 0x200))
#define NO_IMPLEMENTATION() (ASSERT(false))
