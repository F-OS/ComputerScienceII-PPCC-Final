#include "key_press_handler.hpp"

#include <stdexcept>

#include "Keytable.hpp"
#include "message_handler.h"

bool ctrl_q::process_keypress(const KEY_EVENT_RECORD& keypress)
{
    if (keypress.bKeyDown != 0)
    {
        return false;
    }
    if (keypress.wVirtualKeyCode == KEY_PRESS_Q && ((keypress.dwControlKeyState & LEFT_CTRL_PRESSED) != 0u || (
        keypress.dwControlKeyState & RIGHT_CTRL_PRESSED) != 0u))
    {
        call_functions();
        return true;
    }
    return false;
}

void ctrl_q::call_functions()
{
    global_message_handler.set_flag(program_flags::CTRL_Q_PRESSED);
}

bool forward_to_buffer::process_keypress(const KEY_EVENT_RECORD& keypress)
{
    if (keypress.bKeyDown == 0 || (keypress.dwControlKeyState & LEFT_ALT_PRESSED) != 0u
        || (keypress.dwControlKeyState & RIGHT_ALT_PRESSED) != 0u
        || (keypress.dwControlKeyState & LEFT_CTRL_PRESSED) != 0u
        || (keypress.dwControlKeyState & RIGHT_CTRL_PRESSED) != 0u)
    {
        return false;
    }
    for (int i = 0; i <= min(keypress.wRepeatCount / 2, 1); i++)
    {
        call_functions(keypress);
    }
    return true;
}

void forward_to_buffer::call_functions(const KEY_EVENT_RECORD& keypress)
{
    BYTE keybuf[4];
    BYTE keyboard_state[sizeof(KEYARRAY)]{0};
    if (keypress.dwControlKeyState & SHIFT_PRESSED == 1)
    {
        keyboard_state[0x10] = 0x80;
    }
    ToAscii(keypress.wVirtualKeyCode, keypress.wVirtualKeyCode, keyboard_state, reinterpret_cast<LPWORD>(keybuf), 0);
    const std::pair<int, int> cursorstate = global_message_handler.get_cursor();
    if (keypress.uChar.AsciiChar >= 33 && keypress.uChar.AsciiChar <= 126)
    {
        global_message_handler.c_new_message(
                                             keypress.uChar.AsciiChar,
                                             cursorstate.first,
                                             cursorstate.second,
                                             message_tags::BUFFER_MSG_CODE
                                            );
    }
}

bool cursor_movement_related_keys::process_keypress(const KEY_EVENT_RECORD& keypress)
{
    if (keypress.bKeyDown == 0)
    {
        return false;
    }
    if (keypress.wVirtualKeyCode >= KEY_PRESS_LEFT_ARROW && keypress.wVirtualKeyCode <= KEY_PRESS_DOWN_ARROW ||
        keypress.wVirtualKeyCode == KEY_PRESS_ENTER)
    {
        call_functions(keypress);
        return true;
    }
    return false;
}

void cursor_movement_related_keys::call_functions(const KEY_EVENT_RECORD& keypress)
{
    for (int i = 0; i <= min(keypress.wRepeatCount / 2, 1); i++)
    {
        switch (keypress.wVirtualKeyCode)
        {
            case KEY_PRESS_LEFT_ARROW:
                global_message_handler.s_new_message(LEFT_ARROW, message_tags::CURSOR_MOVEMENT_LISTENER);
                break;
            case KEY_PRESS_RIGHT_ARROW:
                global_message_handler.s_new_message(RIGHT_ARROW, message_tags::CURSOR_MOVEMENT_LISTENER);
                break;
            case KEY_PRESS_UP_ARROW:
                global_message_handler.s_new_message(UP_ARROW, message_tags::CURSOR_MOVEMENT_LISTENER);
                break;
            case KEY_PRESS_DOWN_ARROW:
                global_message_handler.s_new_message(DOWN_ARROW, message_tags::CURSOR_MOVEMENT_LISTENER);
                break;
            case KEY_PRESS_ENTER:
                global_message_handler.s_new_message(RETURN_PRESS, message_tags::CURSOR_MOVEMENT_LISTENER);
                break;
            default:
                throw std::runtime_error("Bad input for arrow handler");
        }
    }
}

bool text_position_keys::process_keypress(const KEY_EVENT_RECORD& keypress)
{
    if (keypress.bKeyDown == 0)
    {
        return false;
    }
    if (keypress.wVirtualKeyCode == KEY_PRESS_SPACE || keypress.wVirtualKeyCode == KEY_PRESS_TAB ||
        keypress.wVirtualKeyCode == KEY_PRESS_BK)
    {
        call_functions(keypress);
        return true;
    }
    return false;
}

void text_position_keys::call_functions(const KEY_EVENT_RECORD& keypress)
{
    for (int i = 0; i <= min(keypress.wRepeatCount / 2, 1); i++)
    {
        switch (keypress.wVirtualKeyCode)
        {
            case KEY_PRESS_SPACE:
                global_message_handler.c_new_message(
                                                     ' ',
                                                     global_message_handler.get_cursor().first,
                                                     global_message_handler.get_cursor().second,
                                                     message_tags::BUFFER_MSG_CODE
                                                    );
                break;
            case KEY_PRESS_TAB:
                for (int i = 0; i < 4; i++)
                {
                    global_message_handler.c_new_message(
                                                         ' ',
                                                         global_message_handler.get_cursor().first,
                                                         global_message_handler.get_cursor().second,
                                                         message_tags::BUFFER_MSG_CODE
                                                        );
                }
                break;
            case KEY_PRESS_BK:
                global_message_handler.c_new_message(
                                                     '\b',
                                                     global_message_handler.get_cursor().first,
                                                     global_message_handler.get_cursor().second,
                                                     message_tags::BUFFER_MSG_CODE
                                                    );
                break;
            default:
                throw std::runtime_error("Bad input for arrow handler");
        }
    }
}
