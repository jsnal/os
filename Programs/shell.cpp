/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <unistd.h>

int main(int argc, char** argv)
{
    write(STDOUT_FILENO, "Starting shell\n", 16);
    return 0;
}
