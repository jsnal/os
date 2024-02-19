#include <Kernel/Assert.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/PMM.h>
#include <Kernel/panic.h>

#define DEBUG_TAG "PMM"

PMM::PMM(const multiboot_information_t* multiboot)
{
    ASSERT((u32)&g_kernel_end >= KERNEL_VIRTUAL_BASE);

    u32 physical_region_base = 0;
    u32 physical_region_length = 0;

    dbgprintf("System Memory Map: lower_mem=%d KiB upper_mem=%d MiB\n", multiboot->memory_lower, multiboot->memory_upper / 1024);
    for (u32 position = 0, i = 0; position < multiboot->memory_map_length; position += sizeof(multiboot_mmap_t), i++) {
        multiboot_mmap_t* mmap = multiboot->memory_map + i;

        dbgprintf("  %x%x:%x%x %d (%s)\n",
            (u32)(mmap->base_address >> 32),
            (u32)(mmap->base_address & 0xffffffff),
            (u32)(mmap->length >> 32),
            (u32)(mmap->length & 0xffffffff),
            mmap->type,
            mmap->type == 1 ? "available" : "reserved");

        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE || mmap->base_address < (1 * MB)) {
            continue;
        }

        u32 address_remainder = (u32)(mmap->base_address % PAGE_SIZE);
        u32 length_remainder = (u32)(mmap->length % PAGE_SIZE);

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

        physical_region_base = (u32)(mmap->base_address & 0xffffffff);
        physical_region_length = (u32)(mmap->length & 0xffffffff);

        // TODO: There may be multiple non-contiguous regions that should be mapped.
        break;
    }

    if (physical_region_length == 0) {
        panic("Unable to find any regions to allocate\n");
    }

    if (physical_region_length < KERNEL_ZONE_LENGTH) {
        panic("Unable to find region big enough to allocate the Kernel\n");
    }

    m_kernel_zone = new Zone(physical_region_base, KERNEL_ZONE_LENGTH);
    m_user_zone = new Zone(m_kernel_zone->upper_address().get(),
        physical_region_length - KERNEL_ZONE_LENGTH - (1 * MB));
}
