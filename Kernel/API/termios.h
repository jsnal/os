/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

#define NCCS 32

typedef u32 speed_t;
typedef u32 tcflag_t;
typedef u8 cc_t;

struct termios {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t c_cc[NCCS];
    speed_t c_ispeed;
    speed_t c_ospeed;
};

// Local Modes
#define ECHO 0x1
#define ECHOE 0x2
#define ECHOK 0x4
#define ECHONL 0x8
#define ICANON 0x10
#define IEXTEN 0x20
#define ISIG 0x40
#define NOFLSH 0x80
#define TOPSTOP 0x100
