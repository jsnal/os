/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

struct __DIR {
    int fd;
};

typedef struct __DIR DIR;

struct dirent {
    ino_t d_ino;
    char d_name[];
};

DIR* opendir(const char* name);
int closedir(DIR* dirp);

__END_DECLS
