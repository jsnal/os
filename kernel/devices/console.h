#ifndef SERIAL_H
#define SERIAL_H

#include "api/sys/io.h"
#include <stdbool.h>

#define COM1 0x3f8

/**
 * @brief if the console serial connection is currently enabled
 */
static bool console_enabled = false;

/**
 * @brief enable the console device serial connection
 * 
 * @return true if successful, false otherwise
 */
bool console_enable();

/**
 * @brief put a character to the console device if it's enabled
 * 
 * @param c character to put
 * @return true if console is enabled, false otherwise
 */
bool console_putchar(char c);

#endif