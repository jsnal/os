#include <logger.h>
#include <memory/heap.h>
#include <memory/memory.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <panic.h>

#define DEBUG_TAG "Memory"

static memory_kernel_information_t memory_kernel_information;

void init_memory(physical_address_t* boot_page_directory, const multiboot_information_t* multiboot)
{
    if (!(multiboot->flags & MULTIBOOT_FLAGS_MMAP)) {
        panic("Invalid memory map given by multiboot header\n");
    }

    memory_kernel_information.page_directory = (virtual_address_t*)boot_page_directory;
    memory_kernel_information.page_table = (virtual_address_t*)((uint8_t*)boot_page_directory + PAGE_SIZE);

    init_pmm(&memory_kernel_information, multiboot);

    memory_kernel_information.free_pages = memory_kernel_information.heap + (2 * MB);

    pmm_allocate_frame(ZONE_KERNEL, virtual_to_physical(KERNEL_IMAGE_VIRTUAL_BASE), (memory_kernel_information.bitmap - KERNEL_IMAGE_VIRTUAL_BASE) / PAGE_SIZE);
    init_heap(memory_kernel_information.heap);
    init_paging(memory_kernel_information.page_directory);

    dbgprintf("  kernel_page_directory=%x\n", memory_kernel_information.page_directory);
    dbgprintf("  kernel_page_table=%x\n", memory_kernel_information.page_table);
    dbgprintf("  bitmap=%x\n", memory_kernel_information.bitmap);
    dbgprintf("  heap=%x\n", memory_kernel_information.heap);
    dbgprintf("  free_pages=%x\n", memory_kernel_information.free_pages);
}
