/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Types.h>
#include <limits.h>
#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

typedef struct __DIR DIR;

struct dirent {
    ino_t d_ino;
    u16 d_reclen;
    u8 d_type;
    char d_name[NAME_MAX];
};

struct __DIR {
    int fd;
    dirent last_read_entry;
    u8* buffer;
    size_t size;
    size_t offset;
};

DIR* opendir(const char* name);
dirent* readdir(DIR* dirp);
int closedir(DIR* dirp);

__END_DECLS
