#include <cpu/idt.h>
#include <devices/keyboard.h>
#include <devices/vga.h>
#include <logger.h>
#include <stdbool.h>
#include <sys/io.h>

#define DEBUG_TAG "Keyboard"

#define KEYBOARD_PORT 0x60
#define KEYBOARD_ACK 0x61
#define KEYBOARD_STATUS 0x64

#define KEY_NULL 0
#define KEY_ESCAPE 0x1B
#define KEY_HOME 0xE0
#define KEY_END 0xE1
#define KEY_UP 0xE2
#define KEY_DOWN 0xE3
#define KEY_LEFT 0xE4
#define KEY_RIGHT 0xE5
#define KEY_PAGE_UP 0xE6
#define KEY_PAGE_DOWN 0xE7
#define KEY_INSERT 0xE8
#define KEY_DELETE 0xE9

#define KEYBOARD_MODIFIER_SHIFT 0x01
#define KEYBOARD_MODIFIER_CTRL 0x02
#define KEYBOARD_MODIFIER_ALT 0x04
#define KEYBOARD_MODIFIER_CAPS_LOCK 0x08

static uint32_t keyboard_modifier = 0;

static const char keyboard_map_1[] = {
    KEY_NULL,
    KEY_ESCAPE,
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n', KEY_NULL,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    KEY_NULL,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    KEY_NULL, KEY_NULL, KEY_NULL,
    ' ',
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL,
    KEY_HOME,
    KEY_UP,
    KEY_PAGE_UP,
    '-',
    KEY_LEFT,
    '5',
    KEY_RIGHT,
    '+',
    KEY_END,
    KEY_DOWN,
    KEY_PAGE_DOWN,
    KEY_INSERT,
    KEY_DELETE,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL
};

static const char keyboard_map_2[] = {
    KEY_NULL,
    KEY_ESCAPE,
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    '\n', KEY_NULL,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    KEY_NULL,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    KEY_NULL, KEY_NULL, KEY_NULL,
    ' ',
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL,
    KEY_HOME,
    KEY_UP,
    KEY_PAGE_UP,
    '-',
    KEY_LEFT,
    '5',
    KEY_RIGHT,
    '+',
    KEY_END,
    KEY_DOWN,
    KEY_PAGE_DOWN,
    KEY_INSERT,
    KEY_DELETE,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL, KEY_NULL,
    KEY_NULL, KEY_NULL
};

static uint32_t get_scan_code()
{
    uint32_t code = inb(KEYBOARD_PORT);
    uint32_t value = inb(KEYBOARD_ACK);
    outb(KEYBOARD_ACK, value | 0x80);
    outb(KEYBOARD_ACK, value);
    return code;
}

static void update_modifier(uint8_t modifier, bool pressed)
{
    if (pressed) {
        keyboard_modifier |= modifier;
    } else {
        keyboard_modifier &= ~modifier;
    }
}

static void keyboard_handler()
{
    uint32_t scan_code = get_scan_code();
    bool pressed = (scan_code & 0x80) == 0;
    char converted_character = 0;

#ifdef DEBUG_KEYBOARD
    dbgprintf("Key pressed: modifier=%x scan_code=%d %s\n", keyboard_modifier, scan_code, pressed ? "down" : "up");
#endif

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
            keyboard_modifier ^= KEYBOARD_MODIFIER_CAPS_LOCK;
            break;
        default:
            if (pressed) {
                if (((keyboard_modifier & KEYBOARD_MODIFIER_SHIFT) ^ (keyboard_modifier & KEYBOARD_MODIFIER_CAPS_LOCK)) != 0) {
                    converted_character = keyboard_map_2[scan_code];
                } else {
                    converted_character = keyboard_map_1[scan_code];
                }
                vga_putchar(converted_character);
            }
    }
}

void keyboard_init()
{
    isr_register_handler(ISR_KEYBOARD, keyboard_handler);
}