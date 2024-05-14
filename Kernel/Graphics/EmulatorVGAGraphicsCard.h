/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Bus/PCI.h>
#include <Kernel/Graphics/VGAGraphicsCard.h>
#include <Kernel/Memory/Address.h>
#include <Kernel/Memory/VirtualRegion.h>
#include <Universal/ShareCounted.h>
#include <Universal/SharedPtr.h>

class EmulatorVGAGraphicsCard
    : public VGAGraphicsCard
    , public ShareCounted<EmulatorVGAGraphicsCard> {
public:
    static SharedPtr<EmulatorVGAGraphicsCard> create(Bus::PCI::Address const&, Bus::PCI::ID const&);

    EmulatorVGAGraphicsCard(Bus::PCI::Address const&, Bus::PCI::ID const&);

    void scroll(size_t pixels) override;
    void set_pixel(size_t x, size_t y, u32 value) override;
    void clear(u32 color) override;

    size_t get_width() override;
    size_t get_height() override;

    size_t framebuffer_size() const;

private:
    PhysicalAddress find_framebuffer_address();

    void write_register(u16 index, u16 value);
    u16 read_register(u16 index);

    Result set_resolution(u16 width, u16 height);

    Bus::PCI::Address m_pci_address;
    Bus::PCI::ID m_pci_id;

    UniquePtr<VirtualRegion> m_frame_buffer;

    u16 m_width;
    u16 m_height;
};
