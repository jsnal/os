/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _DEVICES_PIT_
#define _DEVICES_PIT_

#include <stdint.h>

#define TICKS_PER_SECOND 1000

extern uint32_t seconds_since_boot;
extern uint32_t milliseconds_since_boot;

void pit_init();

#endif
