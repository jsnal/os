#include <cpu/idt.h>
#include <logger.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <panic.h>
#include <stdlib.h>

#define DEBUG_TAG "Paging"

static uint32_t* kernel_page_directory = NULL;
static uint32_t* kernel_page_table = NULL;

static void page_fault_isr_handler()
{
    panic("Page fault!\n");
}

void paging_flush_entire_tlb()
{
    asm volatile("mov eax, cr3; \
                  mov cr3, eax");
}

void paging_flush_tlb(const virtual_address_t address)
{
    asm volatile("invlpg [%0]"
                 :
                 : "r"(address));
}

void init_paging(uint32_t* boot_page_directory, const multiboot_information_t* multiboot)
{
    isr_register_handler(14, page_fault_isr_handler);

    kernel_page_directory = boot_page_directory;
    kernel_page_table = (uint32_t*)((uint8_t*)kernel_page_directory + PAGE_SIZE);

    dbgprintf("Initializing paging:\n");
    dbgprintf("  kernel_page_directory=%x\n", kernel_page_directory);
    dbgprintf("  pd0=%x\n", kernel_page_directory[0]);
    dbgprintf("  pd768=%x\n", kernel_page_directory[768]);
    dbgprintf("  kernel_page_table=%x\n", kernel_page_table);
    dbgprintf("  pt0=%x\n", kernel_page_table[0]);
    dbgprintf("  pt1=%x\n", kernel_page_table[1]);
    dbgprintf("  pt2=%x\n", kernel_page_table[2]);

    if (!(multiboot->flags & MULTIBOOT_FLAGS_MMAP)) {
        panic("Invalid memory map given by multiboot header\n");
    }

    init_pmm(multiboot);

    // Unmap the identity mapped region used during boot
    kernel_page_directory[0] = 0;

    // Map the VGA buffer to 0xC0FF3000
    kernel_page_table[1023] = 0x000B8000 | PAGE_PRESENT | PAGE_WRITABLE;

    paging_flush_entire_tlb();
}
