/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Graphics/EmulatorVGAGraphicsCard.h>
#include <Kernel/IO.h>
#include <Kernel/Memory/MemoryManager.h>

#define VBE_DISPLAY_INDEX_ID 0
#define VBE_DISPLAY_INDEX_XRES 1
#define VBE_DISPLAY_INDEX_YRES 2
#define VBE_DISPLAY_INDEX_BPP 3
#define VBE_DISPLAY_INDEX_ENABLE 4
#define VBE_DISPLAY_INDEX_BANK 5
#define VBE_DISPLAY_INDEX_VIRTUAL_WIDTH 6
#define VBE_DISPLAY_INDEX_VIRTUAL_HEIGHT 7
#define VBE_DISPLAY_INDEX_X_OFFSET 8
#define VBE_DISPLAY_INDEX_Y_OFFSET 9

#define VBE_DISPLAY_BPP_4 0x04
#define VBE_DISPLAY_BPP_8 0x08
#define VBE_DISPLAY_BPP_15 0x0F
#define VBE_DISPLAY_BPP_16 0x10
#define VBE_DISPLAY_BPP_24 0x18
#define VBE_DISPLAY_BPP_32 0x20

#define VBE_DISPLAY_DISABLED 0x0u
#define VBE_DISPLAY_ENABLED 0x1u
#define VBE_DISPLAY_LFB_ENABLED 0x40u

#define VBE_DEFAULT_WIDTH 1080
#define VBE_DEFAULT_HEIGHT 720

#define VBE_DISPLAY_IOPORT_INDEX 0x01ce
#define VBE_DISPLAY_IOPORT_DATA 0x01cf

EmulatorVGAGraphicsCard::EmulatorVGAGraphicsCard(Bus::PCI::Address const& address, Bus::PCI::ID const& id)
    : GraphicsCard()
    , m_pci_address(address)
    , m_pci_id(id)
{
    set_resolution(VBE_DEFAULT_WIDTH, VBE_DEFAULT_HEIGHT);

    PhysicalAddress framebuffer_phyiscal_address = find_framebuffer_address();
    size_t framebuffer_size = this->framebuffer_size();

    dbgprintf("EmulatorVGAGraphicsCard", "Framebuffer of size %u located @ 0x%x\n", framebuffer_size, framebuffer_phyiscal_address);
    dbgprintf("EmulatorVGAGraphicsCard", "Framebuffer %ux%u\n", m_width, m_height);

    m_frame_buffer = MemoryManager::the().allocate_kernel_region_at(framebuffer_phyiscal_address, framebuffer_size);

    dbgprintf("EmulatorVGAGraphicsCard", "Frame buffer 0x%x - 0x%x\n", m_frame_buffer->lower(), m_frame_buffer->upper());

    for (size_t x = 0; x < m_width; x++) {
        for (size_t y = 0; y < m_height; y++) {
            m_frame_buffer->lower().ptr()[x + y * m_height] = 'a';
        }
    }
}

SharedPtr<EmulatorVGAGraphicsCard> EmulatorVGAGraphicsCard::create(Bus::PCI::Address const& address, Bus::PCI::ID const& id)
{
    return adopt_shared_ptr(*new EmulatorVGAGraphicsCard(address, id));
}

void EmulatorVGAGraphicsCard::scroll(size_t pixels) { }

void EmulatorVGAGraphicsCard::set_pixel(size_t x, size_t y, u32 value) { }

void EmulatorVGAGraphicsCard::clear() { }

size_t EmulatorVGAGraphicsCard::get_width()
{
    return 0;
}

size_t EmulatorVGAGraphicsCard::get_height()
{
    return 0;
}

size_t EmulatorVGAGraphicsCard::framebuffer_size() const
{
    return m_width * m_height * sizeof(u32);
}

PhysicalAddress EmulatorVGAGraphicsCard::find_framebuffer_address()
{
    return PhysicalAddress(Bus::PCI::read_BAR0(m_pci_address) & 0xfffffff0);
}

void EmulatorVGAGraphicsCard::write_register(u16 index, u16 value)
{
    IO::outw(VBE_DISPLAY_IOPORT_INDEX, index);
    IO::outw(VBE_DISPLAY_IOPORT_DATA, value);
}

u16 EmulatorVGAGraphicsCard::read_register(u16 index)
{
    IO::outw(VBE_DISPLAY_IOPORT_INDEX, index);
    return IO::inw(VBE_DISPLAY_IOPORT_DATA);
}

Result EmulatorVGAGraphicsCard::set_resolution(u16 width, u16 height)
{
    write_register(VBE_DISPLAY_INDEX_ENABLE, VBE_DISPLAY_DISABLED);
    write_register(VBE_DISPLAY_INDEX_XRES, width);
    write_register(VBE_DISPLAY_INDEX_YRES, height);
    write_register(VBE_DISPLAY_INDEX_VIRTUAL_WIDTH, width);
    write_register(VBE_DISPLAY_INDEX_VIRTUAL_HEIGHT, height * 2);
    write_register(VBE_DISPLAY_INDEX_BPP, VBE_DISPLAY_BPP_32);
    write_register(VBE_DISPLAY_INDEX_ENABLE, VBE_DISPLAY_ENABLED | VBE_DISPLAY_LFB_ENABLED);
    write_register(VBE_DISPLAY_INDEX_BANK, 0);

    m_width = width;
    m_height = height;
    return Status::OK;
}
