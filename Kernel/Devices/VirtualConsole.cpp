/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Devices/VirtualConsole.h>
#include <Kernel/Graphics/EmulatorVGAGraphicsCard.h>
#include <Kernel/IO.h>

VirtualConsole::VirtualConsole(u32 const minor)
    : TTYDevice(4, minor)
{
    clear();
    enable_cursor();
}

VirtualConsole::~VirtualConsole()
{
}

void VirtualConsole::set_focused(bool focus)
{
    if (!focus) {
        KeyboardDevice::the().set_keyboard_listener(nullptr);
    } else {
        KeyboardDevice::the().set_keyboard_listener(this);
    }
    m_focused = focus;
}

size_t VirtualConsole::tty_write(const u8* buffer, size_t count)
{
    return put_string(reinterpret_cast<const char*>(buffer), count);
}

void VirtualConsole::tty_echo(char c)
{
    tty_write(reinterpret_cast<const u8*>(&c), 1);
}

void VirtualConsole::handle_key_event(KeyEvent event)
{
    dbgprintf("VirtualConsole", "Keyboard event received: %c - %#x\n", event.character, event.character);

    if (event.is_control_pressed() && event.character >= 'a' && event.character <= 'z') {
        handle_input(event.character - 'a' + 1);
        return;
    }

    auto generate_escape = [&](char c) {
        handle_input('\033');
        handle_input('[');
        handle_input(c);
    };

    switch (event.character) {
        case KEYBOARD_KEY_UP:
            generate_escape('A');
            return;
        case KEYBOARD_KEY_DOWN:
            generate_escape('B');
            return;
        case KEYBOARD_KEY_RIGHT:
            generate_escape('C');
            return;
        case KEYBOARD_KEY_LEFT:
            generate_escape('D');
            return;
        default:
            break;
    }

    handle_input(event.character);
}

void VirtualConsole::scroll(size_t rows)
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

void VirtualConsole::set_color(Color foreground_color, Color background_color)
{
    m_foreground_color = foreground_color;
    m_background_color = background_color;
}

void VirtualConsole::set_cell(size_t row, size_t column, u32 character)
{
    if (row > m_height || column > m_width) {
        return;
    }

    u16 attribute = (m_background_color << 4) | (m_foreground_color & 0x0F);
    m_buffer[row * m_width + column] = character | (attribute << 8);
}

void VirtualConsole::handle_escape_k(const ArrayList<int, kEscapeSequenceMaxParameters>& parameters)
{
    u16 attribute = (Color::Black << 4) | (Color::White & 0x0F);
    u32 mode = 0;
    if (parameters.size() >= 1) {
        mode = parameters[0];
    }

    switch (mode) {
        case 0:
            for (u8 x = m_cursor_column; x < m_width; x++) {
                m_buffer[x + m_cursor_row * m_width] = ' ' | (attribute << 8);
            }
            break;
        case 1:
            for (u8 x = 0; x < m_cursor_column; x++) {
                m_buffer[x + m_cursor_row * m_width] = ' ' | (attribute << 8);
            }
            break;
        case 2:
            clear_row(m_cursor_row);
            break;
    }
}

void VirtualConsole::handle_escape_h(const ArrayList<int, kEscapeSequenceMaxParameters>& parameters)
{
    u8 row = 1;
    u8 column = 1;
    if (parameters.size() >= 1) {
        row = parameters[0];
    }
    if (parameters.size() >= 2) {
        column = parameters[1];
    }

    set_cursor(row - 1, column - 1);
}

void VirtualConsole::handle_escape_c(const ArrayList<int, kEscapeSequenceMaxParameters>& parameters)
{
    u32 columns = 0;
    if (parameters.size() >= 1) {
        columns = parameters[0];
    }

    u8 new_column = m_cursor_column + columns;
    if (new_column > m_width) {
        new_column = m_width;
    }
    set_cursor(m_cursor_row, new_column);
}

void VirtualConsole::handle_escape_j(const ArrayList<int, kEscapeSequenceMaxParameters>& parameters)
{
    u32 mode = 0;
    if (parameters.size() >= 1) {
        mode = parameters[0];
    }

    switch (mode) {
        case 0:
        case 1:
            dbgprintln("VirtualConsole", "Unsupported escape J mode: %d", mode);
            break;
        case 2:
        case 3:
            clear();
            break;
    }
}

void VirtualConsole::handle_escape_sequence(char command)
{
    switch (command) {
        case 'C':
            handle_escape_c(m_escape_sequence_parameters);
            break;
        case 'H':
            handle_escape_h(m_escape_sequence_parameters);
            break;
        case 'J':
            handle_escape_j(m_escape_sequence_parameters);
            break;
        case 'K':
            handle_escape_k(m_escape_sequence_parameters);
            break;
        default:
            dbgprintf("VirtualConsole", "Unhandled escape sequence: %c\n", command);
            break;
    }

    m_escape_sequence_parameters.clear();
    m_escape_sequence_in_parameter = false;
}

void VirtualConsole::put_escape_sequence(char c)
{
    switch (m_escape_sequence_state) {
        case Escape:
            if (c == '[') {
                m_escape_sequence_state = ControlSequenceIntroducer;
            } else {
                m_escape_sequence_state = Text;
            }
            break;
        case ControlSequenceIntroducer:
            if (c >= '0' && c <= '9') {
                if (m_escape_sequence_in_parameter) {
                    m_escape_sequence_parameters.last() = m_escape_sequence_parameters.last() * 10 + (c - '0');
                } else {
                    m_escape_sequence_parameters.add_last(c - '0');
                    m_escape_sequence_in_parameter = true;
                }
            } else if (c == ';') {
                if (!m_escape_sequence_in_parameter) {
                    m_escape_sequence_parameters.add_last(0);
                }
                m_escape_sequence_in_parameter = false;
            } else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                handle_escape_sequence(c);
                m_escape_sequence_state = Text;
            }
            break;
        case Text:
        default:
            break;
    }
}

void VirtualConsole::put_char(char c)
{
    if (m_escape_sequence_state != Text) {
        put_escape_sequence(c);
        return;
    }

    switch (c) {
        case '\0':
            return;
        case '\033':
            m_escape_sequence_state = Escape;
            return;
        case '\r':
            m_cursor_column = 0;
            set_cursor(m_cursor_row, m_cursor_column);
            return;
        case '\n':
            scroll(1);
            set_cursor(m_cursor_row, m_cursor_column);
            return;
    }

    set_cell(m_cursor_row, m_cursor_column, c);
    m_cursor_column++;
    if (m_cursor_column > m_width) {
        scroll(1);
    }
    set_cursor(m_cursor_row, m_cursor_column);
}

size_t VirtualConsole::put_string(const char* string, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        put_char(string[i]);
    }

    return length;
}

void VirtualConsole::clear_row(u8 row)
{
    u16 attribute = (Color::Black << 4) | (Color::White & 0x0F);
    for (u8 x = 0; x < m_width; x++) {
        m_buffer[x + row * m_width] = ' ' | (attribute << 8);
    }
}

void VirtualConsole::clear()
{
    for (u8 y = 0; y < m_height; y++) {
        clear_row(y);
    }
}

void VirtualConsole::enable_cursor()
{
    IO::outb(0x3D4, 0x0A);
    IO::outb(0x3D5, (IO::inb(0x3D5) & 0xC0) | 14);
    IO::outb(0x3D4, 0x0B);
    IO::outb(0x3D5, (IO::inb(0x3D5) & 0xE0) | 15);
    set_cursor(0, 0);
}

void VirtualConsole::disable_cursor()
{
    IO::outb(0x3D4, 0x0A);
    IO::outb(0x3D5, 0x20);
}

void VirtualConsole::set_cursor(u8 row, u8 column)
{
    m_cursor_row = row;
    m_cursor_column = column;
    flush_cursor();
}

void VirtualConsole::flush_cursor()
{
    uint16_t position = m_cursor_column + m_cursor_row * m_width;
    IO::outb(0x3D4, 0x0E);
    IO::outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
    IO::outb(0x3D4, 0x0F);
    IO::outb(0x3D5, (uint8_t)(position & 0xFF));
}
