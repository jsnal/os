/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

DIR* opendir(const char* name)
{
    int fd = open(name, O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        return nullptr;
    }

    DIR* dir = (DIR*)malloc(sizeof(DIR));
    dir->fd = fd;
    return dir;
}

int closedir(DIR* dirp)
{
    if (dirp == nullptr || dirp->fd == -1) {
        return -EBADF;
    }

    // TODO: Add this when close in implemented
    // close(dirp->fd);

    dirp->fd = -1;
    free(dirp);
    return 0;
}
