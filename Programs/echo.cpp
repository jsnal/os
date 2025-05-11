/*
 * Copyright (c) 2025, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/BasicString.h>
#include <Universal/Logger.h>
#include <stdio.h>
#include <string.h>

static void print_with_escape(const char* str)
{
    while (*str) {
        if (*str == '\\') {
            str++;

            if (*str == '0' && (*(str + 1) == '3' || *(str + 2) == '3')) {
                putchar('\033');
                str += 3;
                continue;
            }

            switch (*str) {
                case 'n':
                    putchar('\n');
                    break;
                case 't':
                    putchar('\t');
                    break;
                case '\\':
                    putchar('\\');
                    break;
                case '"':
                    putchar('"');
                    break;
                case '\'':
                    putchar('\'');
                    break;
                default:
                    putchar('\\');
                    putchar(*str);
                    break;
            }
        } else {
            putchar(*str);
        }
        str++;
    }
}

int main(int argc, char** argv)
{
    bool interpret_escapes = false;
    int start_index = 1;

    if (argc > 1 && strncmp(argv[1], "-e", 2) == 0) {
        interpret_escapes = true;
        start_index = 2;
    }

    for (int i = start_index; i < argc; i++) {
        if (interpret_escapes) {
            print_with_escape(argv[i]);
        } else {
            fputs(argv[i], stdout);
        }

        if (i < argc - 1) {
            putchar(' ');
        }
    }
    putchar('\n');
    return 0;
}
