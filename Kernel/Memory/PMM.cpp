#include <Kernel/Assert.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/PMM.h>
#include <Kernel/panic.h>

using namespace Memory;

PMM::PMM(const multiboot_information_t* multiboot)
{
    ASSERT((u32)&g_kernel_end >= KERNEL_VIRTUAL_BASE);

    u32 physical_region_base = 0;
    u32 physical_region_length = 0;

    dbgprintf("PMM", "System Memory Map: lower_mem=%d KiB upper_mem=%d MiB\n", multiboot->memory_lower, multiboot->memory_upper / 1024);
    for (u32 position = 0, i = 0; position < multiboot->memory_map_length; position += sizeof(multiboot_mmap_t), i++) {
        multiboot_mmap_t* mmap = multiboot->memory_map + i;

        dbgprintf("PMM", "  %x%x:%x%x %d (%s)\n",
            (u32)(mmap->base_address >> 32),
            (u32)(mmap->base_address & 0xffffffff),
            (u32)(mmap->length >> 32),
            (u32)(mmap->length & 0xffffffff),
            mmap->type,
            mmap->type == 1 ? "available" : "reserved");

        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE || mmap->base_address < (1 * MB)) {
            continue;
        }

        u32 address_remainder = (u32)(mmap->base_address % Types::PageSize);
        u32 length_remainder = (u32)(mmap->length % Types::PageSize);

        if (address_remainder != 0) {
            dbgprintf("PMM", "  Region does not start on page boundary, correcting by %d bytes\n", address_remainder);
            address_remainder = Types::PageSize - address_remainder;
            mmap->base_address += address_remainder;
            mmap->length -= address_remainder;
        }

        if (length_remainder != 0) {
            dbgprintf("PMM", "  Region does not end on page boundary, correcting by %d bytes\n", length_remainder);
            mmap->length -= length_remainder;
        }

        if (mmap->length < Types::PageSize) {
            dbgprintf("PMM", "  Region is smaller than a page... skipping");
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

    u32 kernel_image_size = (u32)&g_kernel_end - KERNEL_VIRTUAL_BASE;

    m_kernel_zone = new Zone((u32)&g_kernel_end, KERNEL_ZONE_LENGTH - kernel_image_size);
    m_user_zone = new Zone(m_kernel_zone->upper_address(), physical_region_length - KERNEL_ZONE_LENGTH);

    ASSERT(m_kernel_zone != nullptr && m_user_zone != nullptr);
    dbgprintf("PMM", "Initialized PMM: kernel_zone=0x%x user_zone=0x%x\n", m_kernel_zone, m_user_zone);
}
