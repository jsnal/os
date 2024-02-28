/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _DEVICES_PIT_H_
#define _DEVICES_PIT_H_

#include <Universal/Types.h>

#define TICKS_PER_SECOND 1000

namespace PIT {

struct WakeupRoutine {
    u32 milliseconds;
    void (*wakeup_routine)();
};

void register_pit_wakeup(u32, void (*wakeup_routine_pointer)());
uint32_t milliseconds_since_boot();
uint32_t seconds_since_boot();
void init();

}

#endif
