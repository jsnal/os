/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/Types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>

int main(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            perror("cat");
            return -1;
        }

        uint8_t buffer[4 * KB];
        int nread = read(fd, buffer, 4 * KB);
        if (nread < 0) {
            perror("cat");
            return -1;
        }

        int nwritten = write(STDOUT_FILENO, buffer, nread);
        if (nwritten < 0) {
            perror("cat");
            return -1;
        }

        // TODO
        // close(fd);
    }

    return 0;
}
