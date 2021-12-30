#include <api/string.h>
#include <api/printf.h>
#include <cpu/idt.h>
#include <cpu/gdt.h>
#include <devices/vga.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__os__)
#error "Compiling with incorrect toolchain."
#endif

void kernel_main(void)
{
  vga_init();

  gdt_init();

  idt_init();

  /* __asm__("int $0x0"); */

  int a = 10;
  char b[64];
  itoa(b, 64, a, 10);
  vga_write(b);

  vsprintf(vga_putchar, "string: %s\n", "string\0");

  vga_write("Booted!\n");
}
