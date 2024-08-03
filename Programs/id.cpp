/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdint.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    char test_string[22] = "Hello from Userland!\n";
    size_t test_string_length = strlen(test_string);

    int id = get_uid();

    write(STDOUT_FILENO, test_string, test_string_length);

    // while (true)
    //     ;
    return id;
}
