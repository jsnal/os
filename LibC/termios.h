/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdint.h>
#include <sys/cdefs.h>

#define NCCS 11

typedef uint32_t speed_t;
typedef uint32_t tcflag_t;
typedef uint8_t cc_t;

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

// Control Characters
#define VEOF 0
#define VEOL 1
#define VERASE 2
#define VINTR 3
#define VKILL 4
#define VMIN 5
#define VQUIT 6
#define VSTART 7
#define VSTOP 8
#define VSUSP 9
#define VTIME 10

__BEGIN_DECLS

int tcgetattr(int fd, struct termios* termios_p);

__END_DECLS
