/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Universal/PrintFormat.h>
#include <Universal/Stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#define CONVERT_BUFFER_SIZE 64

namespace Universal {

static size_t convert_unsigned_number(unsigned long number, char* buffer, size_t size, int base, bool caps)
{
    const char* digits = caps ? "0123456789ABCDEF" : "0123456789abcdef";
    size_t pos = 0;

    do {
        buffer[pos++] = digits[number % base];
        number /= base;
    } while (number != 0 && pos < size);

    return pos;
}

static size_t convert_signed_number(long number, char* buffer, size_t size, int base, bool caps)
{
    bool negative = false;
    size_t pos = 0;

    if (number < 0) {
        number = -number;
        negative = true;
    }

    pos = convert_unsigned_number(number, buffer, size, base, caps);

    if (negative) {
        buffer[pos++] = '-';
    }
    return pos;
}

static size_t write_unsigned_number(unsigned long number, char* buffer, int base, bool caps, int width, int max)
{
    size_t written = 0;
    size_t convert_size;
    char convert_buffer[CONVERT_BUFFER_SIZE];

    convert_size = convert_unsigned_number(number, convert_buffer, CONVERT_BUFFER_SIZE, base, caps);

    int number_size = convert_size;
    int spaces_size = width - number_size;
    convert_size--;

    for (int i = 0; i < spaces_size && max > 0; i++, max--) {
        *(buffer + written++) = '0';
    }

    for (int i = 0; i < number_size && max > 0; i++, max--) {
        *(buffer + written++) = convert_buffer[convert_size--];
    }

    return written;
}

static size_t write_signed_number(long number, char* buffer, int max)
{
    size_t written = 0;
    int convert_size;
    char convert_buffer[CONVERT_BUFFER_SIZE];

    convert_size = convert_signed_number(number, convert_buffer, CONVERT_BUFFER_SIZE, 10, false) - 1;

    for (; convert_size >= 0 && max > 0; convert_size--, max--) {
        *(buffer + written++) = convert_buffer[convert_size];
    }

    return written;
}

static size_t write_string(const char* string, char* buffer, int max)
{
    int string_length = strlen(string);

    if (buffer == nullptr) {
        return 1;
    }

    if (string_length > max) {
        string_length = max;
    }

    memcpy(buffer, string, string_length);
    buffer[string_length] = '\0';
    return string_length;
}

static size_t write_alternate_form(const char type, char* buffer)
{
    size_t length = 0;
    switch (type) {
        case 'p':
        case 'x':
        case 'X':
            buffer[length++] = '0';
            buffer[length++] = 'x';
            break;
        case 'b':
            buffer[length++] = '0';
            buffer[length++] = 'b';
            break;
    }

    return length;
}

int print_format_buffer(char* str, size_t size, const char* format, va_list ap)
{
    int length = 0;
    int remaining = size;
    unsigned long value_ui;
    signed long value_i;
    char* value_cp;

    for (; *format && remaining > 0; ++format) {
        bool alternate = false;
        u32 width = 0;

        if (*format == '%') {

        keep_parsing:
            ++format;

            if (*format >= '0' && *format <= '9') {
                width *= 10;
                width += *format - '0';
                if (*(format + 1)) {
                    goto keep_parsing;
                }
            }

            if (*format == '#') {
                alternate = true;
                if (*(format + 1)) {
                    goto keep_parsing;
                }
            }

            if (alternate) {
                length += write_alternate_form(*format, &str[length]);
            }

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
                    length += write_string(value_cp, &str[length], remaining);
                    break;
                case 'b':
                    value_ui = va_arg(ap, unsigned int);
                    length += write_unsigned_number(value_ui, &str[length], 2, false, width, remaining);
                    break;
                case 'p':
                case 'x':
                    value_ui = va_arg(ap, unsigned int);
                    length += write_unsigned_number(value_ui, &str[length], 16, false, width, remaining);
                    break;
                case 'X':
                    value_ui = va_arg(ap, unsigned int);
                    length += write_unsigned_number(value_ui, &str[length], 16, true, width, remaining);
                    break;
                case 'u':
                    value_ui = va_arg(ap, unsigned long);
                    length += write_unsigned_number(value_ui, &str[length], 10, false, width, remaining);
                    break;
                case 'd':
                    value_i = va_arg(ap, int);
                    length += write_signed_number(value_i, &str[length], remaining);
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

}
