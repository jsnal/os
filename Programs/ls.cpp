/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    printf("Contents of the current directory:\n");

    DIR* d = opendir(".");
    if (d == nullptr) {
        perror("opendir");
        return -1;
    }

    //    struct dirent* dir;
    //    while ((dir = readdir(d)) != NULL) {
    //        printf("%s\n", dir->d_name);
    //    }
    closedir(d);

    return 0;
}
