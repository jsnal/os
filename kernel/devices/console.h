#ifndef SERIAL_H
#define SERIAL_H

#include <stdbool.h>

#define COM1 0x3f8
#define LINE_STATUS 0x3FD

/**
 * @brief if the console serial connection is currently enabled
 */
static bool console_enabled = false;

/**
 * @brief enable the console device serial connection
 *
 * @return true if successful, false otherwise
 */
bool console_enable_com_port();

/**
 * @brief put a character to the console device if it's enabled
 *
 * @param c character to put
 */
void console_putchar(char c);

/**
 * @brief print a formatted string to the console
 *
 * @return the number of characters printed
 */
int console_printf(const char* format, ...);

#endif
