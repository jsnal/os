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

void init_paging(uint32_t* boot_page_directory, const multiboot_information_t* multiboot)
{
    isr_register_handler(14, page_fault_isr_handler);

    kernel_page_directory = boot_page_directory;
    kernel_page_table = kernel_page_directory + 1024;

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

    pmm_allocate_frame(0x00100000, 8);
    pmm_allocate_frame_first();
    pmm_allocate_frame_first();
    pmm_allocate_frame_first();
    pmm_allocate_frame_first();
    pmm_allocate_frame_first();
    pmm_free_frame(0x00100000);
    pmm_allocate_frame_first();
    pmm_allocate_frame_first();

    for (uint32_t position = 0, i = 0; position < multiboot->memory_map_length; position += sizeof(multiboot_mmap_t), i++) {
        multiboot_mmap_t* mmap = multiboot->memory_map + i;

        // TODO: Take advantage of the first MB of "low memory"
        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE || mmap->base_address < (1 * MB)) {
            continue;
        }

        uint32_t address_remainder = (uint32_t)(mmap->base_address % PAGE_SIZE);
        uint32_t length_remainder = (uint32_t)(mmap->length % PAGE_SIZE);

        if (address_remainder != 0) {
            dbgprintf("  Region does not start on page boundary, correcting by %d bytes\n", address_remainder);
            address_remainder = PAGE_SIZE - address_remainder;
            mmap->base_address += address_remainder;
            mmap->length -= address_remainder;
        }

        if (length_remainder != 0) {
            dbgprintf("  Region does not end on page boundary, correcting by %d bytes\n", length_remainder);
            mmap->length -= length_remainder;
        }

        if (mmap->length < PAGE_SIZE) {
            dbgprintf("  Region is smaller than a page... skipping");
            continue;
        }
    }
}
