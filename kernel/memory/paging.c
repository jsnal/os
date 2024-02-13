#include <cpu/idt.h>
#include <logger.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <panic.h>
#include <stdlib.h>

#define DEBUG_TAG "Paging"

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

uint32_t paging_unmap_kernel_page(virtual_address_t* page_directory, virtual_address_t virtual_address)
{
    uint32_t i;
    uint16_t page_directory_index = PAGE_DIRECTORY_INDEX(virtual_address);
    uint16_t page_table_index = PAGE_TABLE_INDEX(virtual_address);
    physical_address_t page = 0;
    physical_address_t page_table_physical = (physical_address_t)PAGE_ENTRY_ADDR(page_directory[page_directory_index]);
    virtual_address_t* page_table = (virtual_address_t*)(physical_to_virtual(page_table_physical));

    if (virtual_address < KERNEL_VIRTUAL_BASE) {
        dbgprintf("Address out of range\n");
        return MEM_ERR_ADDRESS_OUT_OF_RANGE;
    }

    if ((page_directory[page_directory_index] & PAGE_PRESENT) != 0) {
        if ((page_table[page_table_index] & PAGE_PRESENT) != 0) {
            page = PAGE_ENTRY_ADDR(page_table[page_table_index]);
            page_table[page_table_index] = 0;
            paging_invalidate_page(physical_to_virtual(page));
            pmm_free_frame(ZONE_KERNEL, page);
            dbgprintf("Unmapping entry %d in page table %x\n", page_table_index, page_table);
        }

        for (i = 0; i < 1024; i++) {
            if ((page_table[i] & PAGE_PRESENT) != 0) {
                break;
            }
        }

        if (i == 1024) {
            page_directory[page_directory_index] = 0;
            paging_invalidate_page(physical_to_virtual(page_table_physical));
            pmm_free_frame(ZONE_KERNEL, page_table_physical);
            dbgprintf("Deleting page table at %x\n", page_table);
        }
    }

    return MEM_ERR_NONE;
}

uint32_t paging_map_kernel_page(virtual_address_t* page_directory, virtual_address_t virtual_address, physical_address_t physical_address)
{
    uint16_t page_directory_index = PAGE_DIRECTORY_INDEX(virtual_address);
    uint16_t page_table_index = PAGE_TABLE_INDEX(virtual_address);
    virtual_address_t* page_table = NULL;
    physical_address_t page_table_physical = 0;

    if (virtual_address < KERNEL_VIRTUAL_BASE) {
        dbgprintf("Address out of range\n");
        return MEM_ERR_ADDRESS_OUT_OF_RANGE;
    }

    if (!(page_directory[page_directory_index] & PAGE_PRESENT)) {
        page_table_physical = pmm_allocate_frame_first(ZONE_KERNEL);
        page_table = (virtual_address_t*)physical_to_virtual(PAGE_ENTRY_ADDR(page_table_physical));
        if (MEM_ERR_CHECK(page_table_physical)) {
            dbgprintf("Unable to page in memory %x, error %x\n", physical_address, page_table_physical);
            return page_table_physical;
        }
        page_directory[page_directory_index] = page_table_physical | PAGE_PRESENT | PAGE_WRITABLE;
        memset(page_table, 0, PAGE_SIZE);
        dbgprintf("Created new page table at %x\n", page_table);
    } else {
        page_table = (virtual_address_t*)(physical_to_virtual(PAGE_ENTRY_ADDR(page_directory[page_directory_index])));
    }

    page_table[page_table_index] = physical_address | PAGE_PRESENT | PAGE_WRITABLE;
    paging_invalidate_page(virtual_address);
    dbgprintf("Mapping entry %d in page table %x to %x\n", page_table_index, page_table, physical_address);
    return MEM_ERR_NONE;
}

void init_paging(virtual_address_t* kernel_page_directory)
{
    isr_register_handler(14, page_fault_isr_handler);

    // Unmap the identity mapped region used during boot
    kernel_page_directory[0] = 0;

    // Map the VGA buffer to 0xC03FF000
    paging_map_kernel_page(kernel_page_directory, 0xC03FF000, 0x000B8000);

    for (int i = 0; i < 100; i++) {
        paging_map_kernel_page(kernel_page_directory, 0xC0600000 + (i * PAGE_SIZE), pmm_allocate_frame_first(ZONE_KERNEL));
    }

    paging_flush_tlb();
}
