#include <cpu/idt.h>
#include <logger.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <panic.h>
#include <stdlib.h>

#define DEBUG_TAG "Paging"

paging_kernel_information_t paging_kernel_information;

static void page_fault_isr_handler()
{
    virtual_address_t fault_address;
    asm volatile("mov %0, cr2"
                 : "=r"(fault_address));

    panic("Page fault at %x\n", fault_address);
}

void paging_flush_tlb()
{
    asm volatile("mov eax, cr3; \
                  mov cr3, eax");
}

void paging_invalidate_page(const virtual_address_t address)
{
    asm volatile("invlpg [%0]"
                 :
                 : "r"(address));
}

void paging_map_kernel_page(virtual_address_t virtual_address, physical_address_t physical_address)
{
    uint16_t page_directory_index = PAGE_DIRECTORY_INDEX(virtual_address);
    uint16_t page_table_index = PAGE_TABLE_INDEX(virtual_address);
    virtual_address_t page_table = 0;

    if (virtual_address < KERNEL_VIRTUAL_BASE) {
        dbgprintf("Address out of range\n");
        return;
    }

    if (!(paging_kernel_information.page_directory[page_directory_index] & PAGE_PRESENT)) {
        page_table = pmm_allocate_frame_first(ZONE_KERNEL);
        dbgprintf("page_table=%x\n", page_table);
        if (page_table == PMM_ERROR) {
            return;
        }
        paging_kernel_information.page_directory[page_directory_index] = page_table | PAGE_PRESENT | PAGE_WRITABLE;
        // memset(*(page_table), 0, PAGE_SIZE);
    }

    if (!(paging_kernel_information.page_table[page_table_index] & PAGE_PRESENT)) {
        dbgprintf("putting a new entry in!\n");
        paging_kernel_information.page_table[page_table_index] = physical_address | PAGE_PRESENT | PAGE_WRITABLE;
    }
}

void init_paging(uint32_t* boot_page_directory, const multiboot_information_t* multiboot)
{
    isr_register_handler(14, page_fault_isr_handler);

    paging_kernel_information.page_directory = (virtual_address_t*)boot_page_directory;
    paging_kernel_information.page_table = (virtual_address_t*)((uint8_t*)boot_page_directory + PAGE_SIZE);

    dbgprintf("Initializing paging:\n");

    if (!(multiboot->flags & MULTIBOOT_FLAGS_MMAP)) {
        panic("Invalid memory map given by multiboot header\n");
    }

    init_pmm(&paging_kernel_information, multiboot);

    paging_kernel_information.free_pages = paging_kernel_information.heap + (2 * MB);

    dbgprintf("  kernel_page_directory=%x\n", paging_kernel_information.page_directory);
    dbgprintf("  kernel_page_table=%x\n", paging_kernel_information.page_table);
    dbgprintf("  bitmap=%x\n", paging_kernel_information.bitmap);
    dbgprintf("  heap=%x\n", paging_kernel_information.heap);
    dbgprintf("  free_pages=%x\n", paging_kernel_information.free_pages);

    // Unmap the identity mapped region used during boot
    paging_kernel_information.page_directory[0] = 0;

    // Map the VGA buffer to 0xC03FF000
    paging_kernel_information.page_table[1023] = 0x000B8000 | PAGE_PRESENT | PAGE_WRITABLE;

    // paging_kernel_information.page_table[1023] = 0;
    // paging_map_kernel_page(0xC03FF000, 0x000B8000);
    paging_map_kernel_page(0xC0600000, 0x000B8000);

    paging_flush_tlb();
}
