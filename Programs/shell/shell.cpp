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
            auto read_value = read_result.release_value();
            if (read_value.is_empty()) {
                continue;
            }

            pid_t pid = fork();
            if (pid < 0) {
                dbgprintln("Shell", "Failed to fork parent process");
                return 1;
            } else if (pid == 0) {
                auto args = read_value.split(' ');
                if (args.is_empty()) {
                    return -1;
                }

                char* argv[args.size()];
                for (size_t i = 1; i < args.size(); i++) {
                    argv[i - 1] = args[i].data();
                }
                argv[args.size() - 1] = nullptr;

                if (execve(args.first().data(), argv) == -1) {
                    return -1;
                }
                return 0;
            }

            // TODO: use waitpid instead
            wait(nullptr);
        }
    }

    return 0;
}
