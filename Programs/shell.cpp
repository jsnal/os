/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#define LINE_LENGTH 256

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

static bool readline(const char* prompt, char* line, size_t length)
{
    char c;

    if (!isatty(STDIN_FILENO)) {
        printf("Not a TTY\n");
    } else {
        if (!enable_raw_mode()) {
            return false;
        }

        while (true) {
            read(STDIN_FILENO, &c, 1);
            write(STDOUT_FILENO, &c, 1);
            c = '\0';
        }
    }
    return true;
}

int main(int argc, char** argv)
{
    char line_buffer[LINE_LENGTH];
    printf("Starting shell 0x%x\n", 0x1337);

    pid_t pid = fork();

    getuid();
    printf("Hello world!\n");

    if (pid < 0) {
        return 1;
    } else if (pid == 0) {
        // Child process
        printf("Hello from the child process! PID: %d\n", getpid());
    } else {
        // Parent process
        printf("Hello from the parent process! PID: %d, Child PID: %d\n", getpid(), pid);
    }

    readline("$ ", line_buffer, LINE_LENGTH);
    return 0;
}
