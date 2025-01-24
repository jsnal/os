/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#define NCCS 11

typedef unsigned long speed_t;
typedef unsigned long tcflag_t;
typedef unsigned char cc_t;

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

#define O_RDONLY 0x000000
#define O_WRONLY 0x000001
#define O_RDWR 0x000002
#define O_APPEND 0x000008
#define O_CREAT 0x000200
#define O_TRUNC 0x000400
#define O_EXCL 0x000800
#define O_SYNC 0x002000
#define O_NONBLOCK 0x004000
#define O_NOCTTY 0x008000
#define O_CLOEXEC 0x040000
#define O_NOFOLLOW 0x100000
#define O_DIRECTORY 0x200000
#define O_EXEC 0x400000
#define O_SEARCH O_EXEC

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define MAP_SHARED 0x01
#define MAP_PRIVATE 0x02
#define MAP_ANONYMOUS 0x20
#define MAP_ANON MAP_ANONYMOUS

#define PROT_READ 0x01
#define PROT_WRITE 0x02
#define PROT_EXEC 0x04
#define PROT_NONE 0x00

#define MAP_FAILED ((void*)-1)
