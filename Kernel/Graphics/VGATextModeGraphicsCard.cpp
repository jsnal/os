#include <Kernel/Graphics/VGATextModeGraphicsCard.h>
#include <Kernel/IO.h>
#include <Kernel/Logger.h>

VGATextModeGraphicsCard::VGATextModeGraphicsCard()
    : m_buffer((u16*)0x000B8000)
{
}

SharedPtr<VGATextModeGraphicsCard> VGATextModeGraphicsCard::create()
{
    auto graphics_card = adopt(*new VGATextModeGraphicsCard());
    graphics_card->clear();
    graphics_card->enable_cursor();
    return graphics_card;
}

void VGATextModeGraphicsCard::scroll(size_t rows)
{
    if (m_cursor_row == m_height - 1) {
        memcpy(m_buffer, m_buffer + m_width * rows, (m_height - rows) * m_width * 2);
        for (u8 row = rows; 0 < row; row--) {
            clear_row(m_height - row);
        }
        m_cursor_row = m_height - rows;
    } else {
        m_cursor_row++;
    }
    m_cursor_column = 0;
}

void VGATextModeGraphicsCard::set_color(Color foreground_color, Color background_color)
{
    m_foreground_color = foreground_color;
    m_background_color = background_color;
}

void VGATextModeGraphicsCard::set_pixel(size_t row, size_t column, u32 character)
{
    if (row > m_height || column > m_width) {
        return;
    }

    u16 attribute = (m_background_color << 4) | (m_foreground_color & 0x0F);
    m_buffer[row * m_width + column] = character | (attribute << 8);
}

void VGATextModeGraphicsCard::put_char(u8 character)
{
    switch (character) {
        case '\0':
            return;
        case '\n':
            scroll(1);
            set_cursor(m_cursor_row, m_cursor_column);
            return;
    }

    set_pixel(m_cursor_row, m_cursor_column, character);
    m_cursor_column++;
    if (m_cursor_column >= m_width) {
        scroll(1);
    }
    set_cursor(m_cursor_row, m_cursor_column);
}

void VGATextModeGraphicsCard::put_string(const char* string)
{
    size_t length = strlen(string);
    for (size_t i = 0; i < length; i++) {
        // Skip past ANSI color codes
        if (string[i] == '\x1b') {
            for (size_t j = i; j < length; j++) {
                if (string[j] == 'm') {
                    i = j;
                    break;
                }
            }
            continue;
        }

        put_char(string[i]);
    }
}

void VGATextModeGraphicsCard::clear_row(u8 row)
{
    u16 attribute = (Color::Black << 4) | (Color::White & 0x0F);
    for (u8 x = 0; x < m_width; x++) {
        m_buffer[x + row * m_width] = ' ' | (attribute << 8);
    }
}

void VGATextModeGraphicsCard::clear()
{
    for (u8 y = 0; y < m_height; y++) {
        clear_row(y);
    }
}

void VGATextModeGraphicsCard::enable_cursor()
{
    IO::outb(0x3D4, 0x0A);
    IO::outb(0x3D5, (IO::inb(0x3D5) & 0xC0) | 0);
    IO::outb(0x3D4, 0x0B);
    IO::outb(0x3D5, (IO::inb(0x3D5) & 0xE0) | 15);
    set_cursor(0, 0);
}

void VGATextModeGraphicsCard::disable_cursor()
{
    IO::outb(0x3D4, 0x0A);
    IO::outb(0x3D5, 0x20);
}

void VGATextModeGraphicsCard::set_cursor(u8 row, u8 column)
{
    m_cursor_row = row;
    m_cursor_column = column;
    flush_cursor();
}

void VGATextModeGraphicsCard::flush_cursor()
{
    uint16_t position = m_cursor_column + m_cursor_row * m_width;
    IO::outb(0x3D4, 0x0E);
    IO::outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
    IO::outb(0x3D4, 0x0F);
    IO::outb(0x3D5, (uint8_t)(position & 0xFF));
}
