/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/CPU/IDT.h>

namespace Syscall {

extern "C" void syscall_handler(InterruptFrame&);

}
