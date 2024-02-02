/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <string.h>

static size_t write_string(const char* string, char* buffer)
{
    size_t string_length = strlen(string);

    memcpy(buffer, string, string_length);
    buffer[string_length] = '\0';
    return string_length;
}

int vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
    int length = 0;
    int remaining = size;
    int base = 10;
    long num;
    unsigned int value_ui;
    char* value_cp;

    for (; *format; ++format) {
        if (*format == '%') {
            ++format;

            switch (*format) {
                case '%':
                    str[length++] = '%';
                    break;
                case 'c':
                    value_ui = va_arg(ap, unsigned int);
                    str[length++] = (char)value_ui;
                    break;
                case 's':
                    value_cp = va_arg(ap, char*);
                    length += write_string(value_cp, &str[length]);
                    break;
                case 'b':
                case 'x':
                case 'd':
                    /* num = va_arg(list, int); */
                    /* if (*format == 'b') { */
                    /*     base = 2; */
                    /* } else if (*format == 'x') { */
                    /*     base = 16; */
                    /* } else { */
                    /*     base = 10; */
                    /* } */
                    /*  */
                    /* length += print_int(write, num, base); */
                    break;
            }
        } else {
            str[length++] = *format;
        }

        remaining = size - length;
    }

    str[length] = '\0';
    return length;
}
