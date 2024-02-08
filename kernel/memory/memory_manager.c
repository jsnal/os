#include <cpu/idt.h>
#include <memory/memory_manager.h>
#include <panic.h>

void memory_manager_init()
{

    /* asm volatile("mov eax, cr0; \ */
    /*         or eax, 0x80000001; \ */
    /*         mov cr0, eax"); */
}
