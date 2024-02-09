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

    uint32_t bitmap_length_in_bytes = 0;

    physical_region.bitmap = (uint8_t*)PAGE_ROUND_UP((uint32_t)&kernel_end);

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

        physical_region.base_address = (uint32_t)(mmap->base_address & 0xffffffff);
        physical_region.length = (uint32_t)(mmap->length & 0xffffffff);

        bitmap_length_in_bytes = physical_region.length / PAGE_SIZE / 8;
        memset(physical_region.bitmap, 0, bitmap_length_in_bytes);

        // TODO: There may be multiple non-contiguous regions that should be mapped. physical_region_t should
        //       be a linked-list of regions that are created in this function.
        break;
    }

    if (bitmap_length_in_bytes == 0) {
        panic("Unable to find any regions to allocate\n");
    }

    physical_region.frames_left = physical_region.length / PAGE_SIZE;
    physical_region.last_allocated_frame_index = 0;

    dbgprintf("Initialized PMM: base=0x%x length=%d bitmap=0x%x\n", physical_region.base_address, physical_region.length, physical_region.bitmap);

    // Mark the pages used for the bitmap as used
    pmm_allocate_frame(virtual_to_physical((virtual_address_t)physical_region.bitmap), (bitmap_length_in_bytes + PAGE_SIZE - 1) / PAGE_SIZE);
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

    physical_region.frames_left -= count;

    dbgprintf("Allocated %d physical pages starting at 0x%x\n", count, address);
    return address;
}

physical_address_t pmm_allocate_frame_first()
{
    uint32_t pages_in_space = physical_region.length / PAGE_SIZE;
    uint32_t address;
    uint32_t bitmap_index;
    uint32_t bit_index;

    for (uint32_t i = physical_region.last_allocated_frame_index; i < pages_in_space; i++) {
        bitmap_index = i / 8;
        bit_index = i % 8;

        if (!IS_SET(physical_region.bitmap[bitmap_index], bit_index)) {
            SET_BIT(physical_region.bitmap[bitmap_index], bit_index);
            address = physical_region.base_address + (PAGE_SIZE * i);
            physical_region.frames_left--;
            dbgprintf("Allocated physical page at 0x%x\n", address);
            return address;
        }

        if (i >= pages_in_space) {
            i = 0;
        } else if (i == physical_region.last_allocated_frame_index - 1) {
            dbgprintf("No free pages left!\n");
            return PMM_ERROR;
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
    physical_region.frames_left++;

    dbgprintf("Freed physical page at 0x%x\n", address);
    return 0;
}
