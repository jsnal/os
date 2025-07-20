/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Readline.h>
#include <Universal/Array.h>
#include <Universal/Logger.h>
#include <Universal/StringView.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define LINE_LENGTH 256

static int builtin_cd(const ArrayList<String>& args)
{
    if (args.size() > 2) {
        printf("cd: too many arguments\n");
        return -1;
    }

    if (args.size() == 1) {
        chdir("/home/user");
    } else {
        chdir(args[1].data());
    }

    return 0;
}

static int builtin_pwd(const ArrayList<String>& args)
{
    char path[512];

    if (args.size() > 1) {
        printf("pwd: too many arguments\n");
        return -1;
    }

    if (getcwd(path, 512) == nullptr) {
        printf("pwd: failed to get working directory");
        return -1;
    }

    printf("%s\n", path);
    return 0;
}

static int execute(ArrayList<String>& args)
{
    pid_t pid = fork();
    if (pid < 0) {
        printf("shell: failed to fork\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        char* argv[args.size()];
        for (size_t i = 1; i < args.size(); i++) {
            argv[i - 1] = args[i].data();
        }
        argv[args.size() - 1] = nullptr;

        execve(args.first().data(), argv);

        printf("shell: %s: %s\n", strerror(errno), args.first().data());
        exit(EXIT_FAILURE);
    } else {
        // TODO: use waitpid instead
        wait(nullptr);
    }

    return 0;
}

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

            auto args = read_value.split(' ');
            if (args.is_empty()) {
                return -1;
            }

            if (args[0] == "cd") {
                builtin_cd(args);
            } else if (args[0] == "pwd") {
                builtin_pwd(args);
            } else {
                execute(args);
            }
        }
    }

    return 0;
}
