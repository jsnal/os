#include <Kernel/Devices/KeyboardDevice.h>
#include <Kernel/IO.h>
#include <LibC/errno_defines.h>
#include <Universal/Logger.h>

#define DEBUG_KEYBOARD 0

#define KEYBOARD_PORT 0x60
#define KEYBOARD_ACK 0x61
#define KEYBOARD_STATUS 0x64

static const u8 keyboard_map_1[] = {
    KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_ESCAPE,
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n', KEYBOARD_KEY_NONE,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    KEYBOARD_KEY_NONE,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    ' ',
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_HOME,
    KEYBOARD_KEY_UP,
    KEYBOARD_KEY_PAGE_UP,
    '-',
    KEYBOARD_KEY_LEFT,
    '5',
    KEYBOARD_KEY_RIGHT,
    '+',
    KEYBOARD_KEY_END,
    KEYBOARD_KEY_DOWN,
    KEYBOARD_KEY_PAGE_DOWN,
    KEYBOARD_KEY_INSERT,
    KEYBOARD_KEY_DELETE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE
};

static const u8 keyboard_map_2[] = {
    KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_ESCAPE,
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    '\n', KEYBOARD_KEY_NONE,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    KEYBOARD_KEY_NONE,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    ' ',
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_HOME,
    KEYBOARD_KEY_UP,
    KEYBOARD_KEY_PAGE_UP,
    '-',
    KEYBOARD_KEY_LEFT,
    '5',
    KEYBOARD_KEY_RIGHT,
    '+',
    KEYBOARD_KEY_END,
    KEYBOARD_KEY_DOWN,
    KEYBOARD_KEY_PAGE_DOWN,
    KEYBOARD_KEY_INSERT,
    KEYBOARD_KEY_DELETE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE,
    KEYBOARD_KEY_NONE, KEYBOARD_KEY_NONE
};

KeyboardDevice& KeyboardDevice::the()
{
    static KeyboardDevice s_the;
    return s_the;
}

KeyboardDevice::KeyboardDevice()
    : IRQHandler(IRQ::KEYBOARD)
    , CharacterDevice(13, 1)
{
    enable_irq();
}

ssize_t KeyboardDevice::read(FileDescriptor&, u8* buffer, off_t offset, ssize_t count)
{
    // TODO: Implement this by saving a buffer of the last pressed keys, maybe 512 bytes?
    return 0xdeadbeef;
}

ssize_t KeyboardDevice::write(FileDescriptor&, const u8* buffer, ssize_t count)
{
    return 0;
}

int KeyboardDevice::fstat(FileDescriptor&, stat&)
{
    return -EBADF;
}

int KeyboardDevice::ioctl(FileDescriptor&, uint32_t request, uint32_t* argp)
{
    return 0;
}

void KeyboardDevice::handle_irq(const InterruptRegisters&)
{
    u32 scan_code = get_scan_code();
    bool pressed = (scan_code & 0x80) == 0;
    char converted_character = 0;

    dbgprintf_if(DEBUG_KEYBOARD, "Keyboard", "Key pressed: modifier=%x scan_code=%d %s\n", m_modifier, scan_code, pressed ? "down" : "up");

    switch (scan_code) {
        case 0x2A:
        case 0x36:
        case 0xAA:
        case 0xB6:
            update_modifier(KEYBOARD_MODIFIER_SHIFT, pressed);
            break;
        case 0x1D:
        case 0x9D:
            update_modifier(KEYBOARD_MODIFIER_CTRL, pressed);
            break;
        case 0x38:
        case 0xB8:
            update_modifier(KEYBOARD_MODIFIER_ALT, pressed);
            break;
        case 0x3A:
            m_modifier ^= KEYBOARD_MODIFIER_CAPS_LOCK;
            break;
        default:
            if (pressed) {
                if (((m_modifier & KEYBOARD_MODIFIER_SHIFT) ^ (m_modifier & KEYBOARD_MODIFIER_CAPS_LOCK)) != 0) {
                    converted_character = keyboard_map_2[scan_code];
                } else {
                    converted_character = keyboard_map_1[scan_code];
                }

                if (m_keyboard_listener != nullptr) {
                    KeyEvent event = {
                        .scan_code = static_cast<u16>(scan_code),
                        .character = static_cast<u8>(converted_character),
                        .modifiers = m_modifier
                    };
                    m_keyboard_listener->handle_key_event(event);
                }
            }
    }
}

uint32_t KeyboardDevice::get_scan_code()
{
    u32 code = IO::inb(KEYBOARD_PORT);
    u32 value = IO::inb(KEYBOARD_ACK);
    IO::outb(KEYBOARD_ACK, value | 0x80);
    IO::outb(KEYBOARD_ACK, value);
    return code;
}

void KeyboardDevice::update_modifier(u8 modifier, bool pressed)
{
    if (pressed) {
        m_modifier |= modifier;
    } else {
        m_modifier &= ~modifier;
    }
}
