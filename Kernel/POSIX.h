/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

typedef __UINT64_TYPE__ uint64_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT8_TYPE__ uint8_t;

typedef __INT64_TYPE__ int64_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT8_TYPE__ int8_t;

typedef __UINTPTR_TYPE__ uintptr_t;
typedef __INTPTR_TYPE__ intptr_t;

#define UINT8_MAX (__UINT8_MAX__)
#define UINT16_MAX (__UINT16_MAX__)
#define UINT32_MAX (__UINT32_MAX__)
#define UINT64_MAX (__UINT64_MAX__)

#define INT8_MAX (__INT8_MAX__)
#define INT16_MAX (__INT16_MAX__)
#define INT32_MAX (__INT32_MAX__)
#define INT64_MAX (__INT64_MAX__)

typedef __SIZE_TYPE__ size_t;
typedef int32_t ssize_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef uint32_t ino_t;
typedef int32_t off_t;
typedef int32_t pid_t;
typedef uint32_t dev_t;
typedef uint16_t mode_t;
typedef uint32_t nlink_t;
typedef uint32_t blksize_t;
typedef uint32_t blkcnt_t;
typedef uint32_t time_t;
typedef uint32_t useconds_t;
typedef int32_t suseconds_t;
typedef uint32_t clock_t;
typedef uint32_t speed_t;
typedef uint32_t tcflag_t;
typedef uint8_t cc_t;

#define NCCS 11

struct termios {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t c_cc[NCCS];
    speed_t c_ispeed;
    speed_t c_ospeed;
};

struct stat {
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    clock_t st_atim;
    clock_t st_mtim;
    clock_t st_ctim;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
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
#define MAP_FIXED 0x04
#define MAP_ANONYMOUS 0x08
#define MAP_ANON MAP_ANONYMOUS

#define PROT_NONE 0x00
#define PROT_READ 0x01
#define PROT_WRITE 0x02
#define PROT_EXEC 0x04

#define MAP_FAILED ((void*)-1)
