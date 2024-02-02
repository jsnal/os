#include <api/printf.h>
#include <cpu/panic.h>
#include <logger/logger.h>

void panic(const char* message)
{
    printf_vga("*** PANIC ***\n%s", message);
    errln("*** PANIC ***\n%s", message);
    freeze();
}
