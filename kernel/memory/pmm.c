#include <logger.h>
#include <memory/pmm.h>
#include <panic.h>
#include <stdbool.h>
#include <string.h>

#define DEBUG_TAG "PMM"

static physical_region_t physical_region;

void init_pmm(const multiboot_information_t* multiboot)
{
    if ((uint32_t)&kernel_end < 0xC0000000) {
        panic("Kernel is not in high-half\n");
    }

    if ((uint32_t)&kernel_end % PAGE_SIZE != 0) {
        physical_region.bitmap = (uint8_t*)PAGE_ALIGN((uint32_t)&kernel_end);
    } else {
        physical_region.bitmap = (uint8_t*)&kernel_end;
    }

    dbgprintf("System Memory Map: lower_mem=%d KiB upper_mem=%d MiB\n", multiboot->memory_lower, multiboot->memory_upper / 1024);
    for (uint32_t position = 0, i = 0; position < multiboot->memory_map_length; position += sizeof(multiboot_mmap_t), i++) {
        multiboot_mmap_t* mmap = multiboot->memory_map + i;

        dbgprintf("  %x%x:%x%x %d (%s)\n",
            (uint32_t)(mmap->base_address >> 32),
            (uint32_t)(mmap->base_address & 0xffffffff),
            (uint32_t)(mmap->length >> 32),
            (uint32_t)(mmap->length & 0xffffffff),
            mmap->type,
            mmap->type == 1 ? "available" : "reserved");

        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE || mmap->base_address < (1 * MB)) {
            continue;
        }

        physical_region.base_address = (uint32_t)(mmap->base_address & 0xffffffff);
        physical_region.length = (uint32_t)(mmap->length & 0xffffffff);

        memset(physical_region.bitmap, 0, physical_region.length / PAGE_SIZE / 8);

        // TODO: There may be multiple non-contiguous regions that should be mapped
        break;
    }

    dbgprintf("Initialized PMM: base=0x%x length=%d bitmap=0x%x\n", physical_region.base_address, physical_region.length, physical_region.bitmap);
}

physical_address_t pmm_allocate_frame(const physical_address_t address, const uint32_t count)
{
    if (address < physical_region.base_address || address > physical_region.base_address + physical_region.length) {
        return PMM_ERROR;
    }

    if (address % PAGE_SIZE != 0) {
        dbgprintf("Physical address is not page aligned\n");
        return PMM_ERROR;
    }

    uint32_t address_location = 0;
    uint32_t bitmap_index = 0;
    uint32_t bit_index = 0;

    for (uint32_t i = 0; i < count; i++) {
        address_location = ((address + (PAGE_SIZE * i)) - physical_region.base_address) / PAGE_SIZE;
        bitmap_index = address_location / 8;
        bit_index = address_location % 8;

        if (IS_SET(physical_region.bitmap[bitmap_index], bit_index)) {
            dbgprintf("Unable to allocate page at 0x%x\n", address + (PAGE_SIZE * i));
            // TODO: Unset the bits that were already set if it fails in the middle. This isn't
            //       a important right now because it is only used for initial Kernel memory.
            return PMM_ERROR;
        }

        SET_BIT(physical_region.bitmap[bitmap_index], bit_index);
    }

    dbgprintf("Allocated %d physical pages starting at 0x%x\n", count, address);
    return address;
}

physical_address_t pmm_allocate_frame_first()
{
    uint32_t pages_in_space = physical_region.length / PAGE_SIZE;
    uint32_t address;
    uint32_t bitmap_index;
    uint32_t bit_index;

    for (uint32_t i = 0; i < pages_in_space; i++) {
        bitmap_index = i / 8;
        bit_index = i % 8;

        if (!IS_SET(physical_region.bitmap[bitmap_index], bit_index)) {
            SET_BIT(physical_region.bitmap[bitmap_index], bit_index);
            address = physical_region.base_address + (PAGE_SIZE * i);
            dbgprintf("Allocated physical page at 0x%x\n", address);
            return address;
        }
    }

    dbgprintf("No free pages left!\n");
    return PMM_ERROR;
}

uint32_t pmm_free_frame(const physical_address_t address)
{
    if (address < physical_region.base_address || address > physical_region.base_address + physical_region.length) {
        return PMM_ERROR;
    }

    if (address % PAGE_SIZE != 0) {
        dbgprintf("Physical address is not page aligned\n");
        return PMM_ERROR;
    }

    uint32_t address_location = (address - physical_region.base_address) / PAGE_SIZE;
    uint32_t bitmap_index = address_location / 8;
    uint32_t bit_index = address_location % 8;

    CLEAR_BIT(physical_region.bitmap[bitmap_index], bit_index);
    dbgprintf("Freed physical page at 0x%x\n", address);
    return 0;
}
