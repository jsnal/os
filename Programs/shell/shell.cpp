/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Readline.h>
#include <Universal/Array.h>
#include <Universal/StringView.h>
#include <stdio.h>
#include <unistd.h>

#define LINE_LENGTH 256

int main(int argc, char** argv)
{
    printf("Starting shell 0x%x\n", 0x1337);

    // pid_t pid = fork();
    //    if (pid < 0) {
    //        return 1;
    //    } else if (pid == 0) {
    //        //        const char* execve_program = "/bin/id";
    //        //        char* const execve_argv[] = { nullptr };
    //        //        char* const execve_envp[] = { nullptr };

    //        //        if (execve(execve_program, execve_argv, execve_envp) == -1) {
    //        //            return -1;
    //        //        }

    //        printf("Hello from the child process! PID: %d\n", getpid());
    //        return 0;
    //    } else {
    //        printf("Hello from the parent process! PID: %d, Child PID: %d\n", getpid(), pid);
    //    }

    while (true) {
        Readline readline(STDIN_FILENO, STDOUT_FILENO);
        auto read_result = readline.read("$ ");
        if (read_result.is_ok()) {
            printf("Typed: '%s'\n", read_result.release_value().str());
        }
    }

    return 0;
}
