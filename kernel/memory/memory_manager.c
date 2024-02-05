#include <cpu/idt.h>
#include <memory/memory_manager.h>
#include <panic.h>

static void page_fault_isr_handler()
{
    panic("Page fault!");
}

void memory_manager_init()
{
    isr_register_handler(14, page_fault_isr_handler);

    /* asm volatile("mov eax, cr0; \ */
    /*         or eax, 0x80000001; \ */
    /*         mov cr0, eax"); */
}
