#include <api/printf.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <devices/vga.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__os__)
#error "Compiling with incorrect toolchain."
#endif

void kernel_main(void)
{
  gdt_init();

  idt_init();

  vga_init();


  /* printf_vga("number: %d\nstring: %s\n", 847358, "really long string thing"); */

  __asm__ __volatile__("int $0");
//  __asm__ __volatile__("int $3");

  printf_vga("Booted!!\n");

  for(;;);

  /* __asm__ volatile("mov $0xFF, %eax"); */
}
