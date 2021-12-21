#include "devices/vga.h"

static vga_terminal terminal;

static inline uint8_t vga_entry_color(vga_color fg, vga_color bg)
{
  return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
  return (uint16_t) uc | (uint16_t) color << 8;
}

void vga_init()
{
  terminal.row = 0;
  terminal.column = 0;
  terminal.color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal.buffer = (uint16_t*) 0xB8000;

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      terminal.buffer[y * WIDTH + x] = vga_entry(' ', terminal.color);
    }
  }
}

static void vga_putentryat(char c, uint8_t color, size_t x, size_t y)
{
  terminal.buffer[y * WIDTH + x] = vga_entry(c, color);
}

void vga_putchar(char c)
{
  if (c == '\n') {
    terminal.row++;
    terminal.column = 0;
    return;
  }

  vga_putentryat(c, terminal.color, terminal.column, terminal.row);

  if (++terminal.column == WIDTH) {
    terminal.column = 0;
    if (++terminal.row == HEIGHT) {
      terminal.row = 0;
    }
  }
}

void vga_nwrite(const char* data, size_t size)
{
  for (size_t i = 0; i < size; i++) {
    vga_putchar(data[i]);
  }
}

void vga_write(const char* data)
{
  vga_nwrite(data, strlen(data));
}
