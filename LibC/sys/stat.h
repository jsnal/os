/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

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

int fstat(int fd, struct stat* statbuf);

__END_DECLS
