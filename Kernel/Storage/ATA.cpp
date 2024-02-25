#include <Kernel/Assert.h>
#include <Kernel/CPU/IDT.h>
#include <Kernel/IO.h>
#include <Kernel/Logger.h>
#include <Kernel/Storage/ATA.h>

#include <Kernel/kmalloc.h>

#define IDE0_DATA 0x1F0
#define IDE0_STATUS 0x1F7
#define IDE0_COMMAND 0x1F7
#define BUSY 0x80
#define IDENTIFY_DRIVE 0xEC

namespace Storage::ATA {

struct ATAItdentifyInfo {
    u16 general_configuration;
    u16 number_of_cylinders;
    u16 specific_configuration;
    u16 number_of_heads;
    u16 retired1[2];
    u16 num_of_sectors_per_track;
    u16 vendor_unique1[3];
    u8 serial_number[20];
    u16 retired2[2];
    u16 obsolete1;
    u8 firmware_revision[8];
    u8 model_number[40];
    u8 maximum_block_transfer;
    u8 vendor_unique2;
};

static void disk_interrupt_handler()
{
    dbgprintf("ATA", "Received an interrupt!\n");
}

void init()
{
    cli();
    IDT::register_interrupt_handler(ISR_DISK, disk_interrupt_handler);

    while (IO::inb(IDE0_STATUS) & BUSY)
        ;

    IO::outb(0x1F6, 0xA0);
    IO::outb(IDE0_COMMAND, IDENTIFY_DRIVE);
    IO::outb(IDE0_COMMAND, IDENTIFY_DRIVE);
    IO::outb(IDE0_COMMAND, IDENTIFY_DRIVE);
    IO::outb(IDE0_COMMAND, IDENTIFY_DRIVE);

    // ASSERT(IO::inb(IDE0_STATUS) != 0);

    // u8* buffer = new u8[256];
    u8 wbuffer[512];
    u8 bbuffer[512];
    u16* w = (u16*)wbuffer;
    u8* b = bbuffer;

    for (u32 i = 0; i < 256; i++) {
        u32 val = IO::inw(IDE0_DATA);
        *(w++) = val;
        *(b++) = (val >> 8) & 0xFF;
        *(b++) = val & 0xFF;
        // *((u8*)bbuffer + i) = (val >> 8) & 0xFF;
        // *((u8*)bbuffer + i + 1) = val & 0xFF;
        // *((u32*)(wbuffer + i * 2)) = val;
    }

    for (u32 i = 93; i > 54 && bbuffer[i] == ' '; --i) {
        bbuffer[i] = 0;
    }

    ATAItdentifyInfo* ata_info = (ATAItdentifyInfo*)wbuffer;

    dbgprintf("ATA", "ide0: Master=?? C/H/Spt=%u/%u/%u\n",
        ata_info->number_of_cylinders, ata_info->number_of_heads, ata_info->num_of_sectors_per_track);

    dbgprintf("ATA", "serial %s\n", bbuffer + 54);

    sti();
}
}
