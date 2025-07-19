/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
struct [[gnu::packed]] sys_dirent {
    u32 inode;
    u16 size;
    u8 name_length;
    u8 type;
    char name[];
};

DIR* opendir(const char* name)
{
    int fd = open(name, O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        return nullptr;
    }

    DIR* dir = (DIR*)malloc(sizeof(DIR));
    dir->fd = fd;
    dir->buffer = nullptr;
    dir->size = 0;
    return dir;
}

dirent* readdir(DIR* dirp)
{
    stat st;
    if (fstat(dirp->fd, &st) < 0) {
        errno = EIO;
        return nullptr;
    }

    if (dirp->buffer == nullptr) {
        dirp->size = st.st_size;
        dirp->offset = 0;
        dirp->buffer = (u8*)malloc(st.st_size);

        if (syscall(SYS_getdirentries, (int)dirp->fd, (int)dirp->buffer, (int)dirp->size) < 0) {
            free(dirp->buffer);
            dirp->buffer = nullptr;
            dirp->size = 0;
            errno = EIO;
            return nullptr;
        }
    }

    if (dirp->offset >= dirp->size) {
        return nullptr;
    }

    auto* entry = reinterpret_cast<sys_dirent*>(dirp->buffer + dirp->offset);
    dirp->offset += entry->size;

    dirp->last_read_entry.d_ino = entry->inode;
    dirp->last_read_entry.d_reclen = entry->size;
    dirp->last_read_entry.d_type = entry->type;
    for (int i = 0; i < entry->name_length; i++) {
        dirp->last_read_entry.d_name[i] = entry->name[i];
    }
    dirp->last_read_entry.d_name[entry->name_length] = '\0';

    return &dirp->last_read_entry;
}

int closedir(DIR* dirp)
{
    if (dirp == nullptr || dirp->fd == -1) {
        return -EBADF;
    }

    // TODO: Add this when close in implemented
    // close(dirp->fd);

    free(dirp->buffer);
    dirp->buffer = nullptr;
    dirp->fd = -1;
    free(dirp);
    return 0;
}
