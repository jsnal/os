/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int id = getuid();
    printf("uid=%u\n", id);

    int fd = open("/home/user/file.txt", O_RDONLY);
    printf("file opened @ %d\n", fd);

    uint8_t buffer[1024];
    read(fd, buffer, 1024);

    printf("%s\n", buffer);

    return 0;
}
