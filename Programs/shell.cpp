/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <termios.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    write(STDOUT_FILENO, "Starting shell\n", 16);

    struct termios original_termios;
    tcgetattr(STDIN_FILENO, &original_termios);

    return 0;
}
