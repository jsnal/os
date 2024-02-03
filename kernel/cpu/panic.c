#include <cpu/panic.h>
#include <devices/vga.h>
#include <logger.h>

void panic(const char* message)
{
    vga_printf("*** PANIC ***\n%s", message);
    errprintf("*** PANIC ***\n%s", message);
    freeze();
}
