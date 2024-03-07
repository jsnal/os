/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _DEVICES_PIT_H_
#define _DEVICES_PIT_H_

#include <Kernel/CPU/IRQHandler.h>
#include <Universal/Types.h>

#define TICKS_PER_SECOND 1000
#define WAKEUP_ROUTINES_COUNT 2

struct WakeupRoutine {
    u32 milliseconds;
    void (*wakeup_routine)();
};

class PIT final : public IRQHandler {
public:
    static PIT& the();

    PIT();

    static u32 milliseconds_since_boot() { return s_milliseconds_since_boot; }

    static u32 seconds_since_boot() { return s_seconds_since_boot; }

    void register_pit_wakeup(u32 milliseconds, void (*wakeup_routine_pointer)());

private:
    void handle() override;

    static u32 s_milliseconds_since_boot;
    static u32 s_seconds_since_boot;

    u8 m_wakeup_routine_count { 0 };
    WakeupRoutine m_wakeup_routines[WAKEUP_ROUTINES_COUNT] {};
};

#endif
