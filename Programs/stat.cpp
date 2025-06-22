/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("stat: missing operand\n");
        return 1;
    }

    char* filename = argv[1];

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct stat stat { };
    if (fstat(fd, &stat) == -1) {
        perror("fstat");
        return 1;
    }

    printf("File: %s\n", filename);
    printf("Size: %u\n", stat.st_size);
    printf("Blocks: %u\n", stat.st_blocks);
    printf("IO Block: %u\n", stat.st_blksize);
    printf("Device: %d\n", stat.st_dev);
    printf("Inode: %d\n", stat.st_ino);
    printf("Links: %d\n", stat.st_nlink);

    return 0;
}
