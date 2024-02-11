#include <logger.h>
#include <memory/zone.h>
#include <stdlib.h>

#define DEBUG_TAG "Zone"
#define SET_BIT(b, i) b = b | (1 << (i % CHAR_BIT))
#define CLEAR_BIT(b, i) b = (b) & (~(1 << (i % CHAR_BIT)))
#define IS_SET(b, i) ((b >> (i % CHAR_BIT)) & 1)

physical_address_t zone_allocate_frame(physical_zone_t* zone, const physical_address_t address, const uint32_t count)
{
    if (zone == NULL || address < zone->base_address || address > zone->base_address + zone->length) {
        return ZONE_ERROR;
    }

    if (address % PAGE_SIZE != 0) {
        dbgprintf("Physical address is not page aligned\n");
        return ZONE_ERROR;
    }

    uint32_t address_location = 0;
    uint32_t bitmap_index = 0;
    uint32_t bit_index = 0;

    for (uint32_t i = 0; i < count; i++) {
        address_location = ((address + (PAGE_SIZE * i)) - zone->base_address) / PAGE_SIZE;
        bitmap_index = address_location / 8;
        bit_index = address_location % 8;

        if (IS_SET(zone->bitmap[bitmap_index], bit_index)) {
            dbgprintf("Unable to allocate page at 0x%x\n", address + (PAGE_SIZE * i));
            // TODO: Unset the bits that were already set if it fails in the middle. This isn't
            //       a important right now because it is only used for initial Kernel memory.
            return ZONE_ERROR;
        }

        SET_BIT(zone->bitmap[bitmap_index], bit_index);
    }

    zone->frames_left -= count;

    dbgprintf("Allocated %d physical pages starting at 0x%x\n", count, address);
    return address;
}

physical_address_t zone_allocate_frame_first(physical_zone_t* zone)
{
    if (zone == NULL) {
        return ZONE_ERROR;
    }

    uint32_t pages_in_space = zone->length / PAGE_SIZE;
    uint32_t address;
    uint32_t bitmap_index;
    uint32_t bit_index;

    for (uint32_t i = zone->last_allocated_frame_index; i < pages_in_space; i++) {
        bitmap_index = i / 8;
        bit_index = i % 8;

        if (!IS_SET(zone->bitmap[bitmap_index], bit_index)) {
            SET_BIT(zone->bitmap[bitmap_index], bit_index);
            address = zone->base_address + (PAGE_SIZE * i);
            zone->frames_left--;
            dbgprintf("Allocated physical page at 0x%x\n", address);
            return address;
        }

        if (i >= pages_in_space) {
            i = 0;
        } else if (i == zone->last_allocated_frame_index - 1) {
            dbgprintf("No free pages left!\n");
            return ZONE_ERROR;
        }
    }

    dbgprintf("No free pages left!\n");
    return ZONE_ERROR;
}

uint32_t zone_free_frame(physical_zone_t* zone, const physical_address_t address)
{
    if (zone == NULL || address < zone->base_address || address > zone->base_address + zone->length) {
        return ZONE_ERROR;
    }

    if (address % PAGE_SIZE != 0) {
        dbgprintf("Physical address is not page aligned\n");
        return ZONE_ERROR;
    }

    uint32_t address_location = (address - zone->base_address) / PAGE_SIZE;
    uint32_t bitmap_index = address_location / 8;
    uint32_t bit_index = address_location % 8;

    CLEAR_BIT(zone->bitmap[bitmap_index], bit_index);
    zone->frames_left++;

    dbgprintf("Freed physical page at 0x%x\n", address);
    return 0;
}

void zone_dump(physical_zone_t* zone, physical_zone_type_e zone_type)
{
    if (zone_type == ZONE_USER) {
        dbgprintf("User zone:\n");
    } else {
        dbgprintf("Kernel zone:\n");
    }
    dbgprintf("  base_address=%x length=%d\n", zone->base_address, zone->length);
    dbgprintf("  bitmap=%x bitmap_length=%d frames_left=%d\n", zone->bitmap, zone->bitmap_length, zone->frames_left);
}
