/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _DEVICES_PIT_H_
#define _DEVICES_PIT_H_

#include <stdint.h>

#define TICKS_PER_SECOND 1000

namespace PIT {

uint32_t milliseconds_since_boot();
uint32_t seconds_since_boot();
void init();

}

#endif
