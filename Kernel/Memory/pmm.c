#include <logger.h>
#include <memory/pmm.h>
#include <panic.h>
#include <stdbool.h>
#include <string.h>

#define DEBUG_TAG "PMM"

static physical_zone_t kernel_zone;
static physical_zone_t user_zone;

void init_pmm(memory_kernel_information_t* memory_kernel_information, const multiboot_information_t* multiboot)
{
    if ((uint32_t)&kernel_end < 0xC0000000) {
        panic("Kernel is not in high-half\n");
    }

    uint32_t physical_region_base = 0;
    uint32_t physical_region_length = 0;
    uint32_t number_of_pages = 0;

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

        physical_region_base = (uint32_t)(mmap->base_address & 0xffffffff);
        physical_region_length = (uint32_t)(mmap->length & 0xffffffff);

        // TODO: There may be multiple non-contiguous regions that should be mapped.
        break;
    }

    if (physical_region_length == 0) {
        panic("Unable to find any regions to allocate\n");
    }

    if (physical_region_length < KERNEL_ZONE_LENGTH) {
        panic("Unable to find region big enough to allocate the Kernel\n");
    }

    kernel_zone.base_address = physical_region_base;
    kernel_zone.length = KERNEL_ZONE_LENGTH;
    kernel_zone.bitmap = (uint8_t*)PAGE_ROUND_UP((uint32_t)&kernel_end);
    kernel_zone.bitmap_length = KERNEL_ZONE_LENGTH / PAGE_SIZE / 8;
    kernel_zone.frames_left = KERNEL_ZONE_LENGTH / PAGE_SIZE;
    kernel_zone.last_allocated_frame_index = 0;
    memset(kernel_zone.bitmap, 0, kernel_zone.bitmap_length);

    user_zone.base_address = kernel_zone.base_address + KERNEL_ZONE_LENGTH;
    user_zone.length = physical_region_length - KERNEL_ZONE_LENGTH;
    user_zone.bitmap = kernel_zone.bitmap + kernel_zone.bitmap_length;
    user_zone.bitmap_length = user_zone.length / PAGE_SIZE / 8;
    user_zone.frames_left = user_zone.length / PAGE_SIZE;
    user_zone.last_allocated_frame_index = 0;
    memset(user_zone.bitmap, 0, user_zone.bitmap_length);

    zone_dump(&kernel_zone, ZONE_KERNEL);
    zone_dump(&user_zone, ZONE_USER);

    number_of_pages = ((kernel_zone.bitmap_length + user_zone.bitmap_length) + PAGE_SIZE - 1) / PAGE_SIZE;
    pmm_allocate_frame(ZONE_KERNEL, virtual_to_physical((virtual_address_t)kernel_zone.bitmap), number_of_pages);

    memory_kernel_information->bitmap = (virtual_address_t)(kernel_zone.bitmap);
    memory_kernel_information->heap = (virtual_address_t)(kernel_zone.bitmap + (number_of_pages * PAGE_SIZE));
}

physical_address_t pmm_allocate_frame(physical_zone_type_e zone_type, const physical_address_t address, const uint32_t count)
{
    if (zone_type == ZONE_USER) {
        return zone_allocate_frame(&user_zone, address, count);
    }

    return zone_allocate_frame(&kernel_zone, address, count);
}

physical_address_t pmm_allocate_frame_first(physical_zone_type_e zone_type)
{
    if (zone_type == ZONE_USER) {
        return zone_allocate_frame_first(&user_zone);
    }

    return zone_allocate_frame_first(&kernel_zone);
}

uint32_t pmm_free_frame(physical_zone_type_e zone_type, const physical_address_t address)
{
    if (zone_type == ZONE_USER) {
        return zone_free_frame(&user_zone, address);
    }

    return zone_free_frame(&kernel_zone, address);
}
