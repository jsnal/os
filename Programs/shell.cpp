/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

static struct termios original_termios;

static bool enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &original_termios);

    // Input modes: no break, no CR to NL, no parity check, no strip char,
    // no start/stop output control.
    original_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    // Output modes - disable post processing
    original_termios.c_oflag &= ~(OPOST);

    // Control modes - set 8 bit chars
    original_termios.c_cflag |= (CS8);

    // Local modes - choing off, canonical off, no extended functions,
    // no signal chars (^Z,^C)
    original_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    // Control chars - set return condition: min number of bytes and timer.
    // We want read to return every single byte, without timeout.
    original_termios.c_cc[VMIN] = 1;

    // 1 byte, no timer
    original_termios.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) < 0) {
        return false;
    }

    return true;
}

int main(int argc, char** argv)
{
    printf("Starting shell 0x%x\n", 0x1337);

    enable_raw_mode();
    return 0;
}
