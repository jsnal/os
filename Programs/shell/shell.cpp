/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Readline.h>
#include <Universal/Array.h>
#include <Universal/Logger.h>
#include <Universal/StringView.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define LINE_LENGTH 256

int main(int argc, char** argv)
{
    printf("Starting Shell\n");

    while (true) {
        Readline readline(STDIN_FILENO, STDOUT_FILENO, "$ ");
        auto read_result = readline.read();
        if (read_result.is_ok()) {
            printf("\033[31mTyped:\033[0m '%s'\n", read_result.release_value().str());

            auto read_value = read_result.release_value();
            if (read_value.is_empty()) {
                continue;
            }

            pid_t pid = fork();
            if (pid < 0) {
                dbgprintln("Shell", "Failed to fork parent process");
                return 1;
            } else if (pid == 0) {
                char* const execve_argv[] = { "-e", "test", "many", "args", nullptr };
                if (execve(read_result.release_value().str(), execve_argv) == -1) {
                    return -1;
                }
                return 0;
            }

            int waited_pid = wait(nullptr);
            printf("pid=%d exited\n", waited_pid);
        }
    }

    return 0;
}
