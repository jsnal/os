/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/CPU/IRQHandler.h>
#include <Kernel/Devices/CharacterDevice.h>
#include <Universal/Types.h>

#define KEYBOARD_MODIFIER_SHIFT 0x01
#define KEYBOARD_MODIFIER_CTRL 0x02
#define KEYBOARD_MODIFIER_ALT 0x04
#define KEYBOARD_MODIFIER_CAPS_LOCK 0x08

struct KeyEvent {
    u16 scan_code;
    u8 key;
    u8 character;
    u8 modifiers;

    bool is_shift_pressed() const { return modifiers & KEYBOARD_MODIFIER_SHIFT; }
    bool is_control_pressed() const { return modifiers & KEYBOARD_MODIFIER_CTRL; }
    bool is_alt_pressed() const { return modifiers & KEYBOARD_MODIFIER_ALT; }
    bool is_caps_lock_pressed() const { return modifiers & KEYBOARD_MODIFIER_CAPS_LOCK; }
};

class KeyboardListener {
public:
    virtual ~KeyboardListener() = default;
    virtual void handle_key_event(KeyEvent) = 0;
};

class KeyboardDevice final
    : public CharacterDevice
    , public IRQHandler {
public:
    static KeyboardDevice& the();

    KeyboardDevice();

    ssize_t read(FileDescriptor&, u8* buffer, off_t offset, ssize_t count) override;
    ssize_t write(FileDescriptor&, const u8* buffer, ssize_t count) override;
    int ioctl(FileDescriptor&, uint32_t request, uint32_t* argp) override;

    void set_keyboard_listener(KeyboardListener* keyboard_listener) { m_keyboard_listener = keyboard_listener; }

private:
    void handle_irq(const InterruptRegisters&) override;

    u32 get_scan_code();

    void update_modifier(uint8_t modifier, bool pressed);

    u8 m_modifier { 0 };

    KeyboardListener* m_keyboard_listener { nullptr };
};
