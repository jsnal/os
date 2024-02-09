#ifndef DEVICES_VGA_H
#define DEVICES_VGA_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define WIDTH 80
#define HEIGHT 25
#define VGA_BUFFER_SIZE 128
#define VGA_BUFFER_LOCATION 0xC03FF000

typedef enum vga_color_enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
} vga_color;

typedef struct vga_terminal_struct {
    size_t row;
    size_t column;
    uint8_t color;
    uint16_t* buffer;
} vga_terminal;

void vga_init();

void vga_clear();

void vga_putchar(char c);

void vga_nwrite(const char* data, size_t size);

void vga_write(const char* data);

void vga_printf(const char* format, ...);

#endif
