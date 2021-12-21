#include <api/string.h>
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

  vga_write("Booted!\n");
}
