#include "key_press_handler.hpp"

#include <stdexcept>

key_press_handler::key_press_handler(dispatch& disp, bool ctrl, bool alt, int key)
{
    req_keypress = key;
    req_ctrl_key = ctrl;
    req_alt_key = alt;
    dispatcher = &disp;
}

dispatch* key_press_handler::get_dispatcher()
{
    return dispatcher;
}

bool key_press_handler::process_keypress(KEY_EVENT_RECORD keypress)
{
    if (keypress.bKeyDown != 0)
    {
        return false;
    }
    if (keypress.wVirtualKeyCode == req_keypress)
    {
        if (req_alt_key && (((keypress.dwControlKeyState & LEFT_ALT_PRESSED)) || ((keypress.dwControlKeyState &
            RIGHT_ALT_PRESSED))))
        {
            call_functions();
            return true;
        }
        if (req_ctrl_key && (((keypress.dwControlKeyState & LEFT_CTRL_PRESSED)) || ((keypress.dwControlKeyState &
            RIGHT_CTRL_PRESSED))))
        {
            call_functions();
            return true;
        }
    }
    return false;
}

void ctrl_q::call_functions()
{
    get_dispatcher()->return_to_main();
}

bool forward_to_buffer::process_keypress(KEY_EVENT_RECORD keypress)
{
    if (keypress.bKeyDown != 0)
    {
        return false;
    }
    if ((keypress.wVirtualKeyCode >= KEY_PRESS_0 && keypress.wVirtualKeyCode <= KEY_PRESS_9) || (keypress.
        wVirtualKeyCode >= KEY_PRESS_A && keypress.wVirtualKeyCode <= KEY_PRESS_Z))
    {
        if (((keypress.dwControlKeyState & LEFT_ALT_PRESSED) != 0u)
            || ((keypress.dwControlKeyState & RIGHT_ALT_PRESSED) != 0u)
            || ((keypress.dwControlKeyState & LEFT_CTRL_PRESSED) != 0u)
            || ((keypress.dwControlKeyState & RIGHT_CTRL_PRESSED) != 0u))
        {
            return false; // We don't handle keypresseses with ctrl/alt keys.
        }
        call_functions(keypress);
        return true;
    }
    return false;
}

void forward_to_buffer::call_functions(const KEY_EVENT_RECORD& keypress)
{
    BYTE keybuf[4];
    BYTE keyboard_state[sizeof(KEYARRAY)];
    if (!GetKeyboardState(keyboard_state))
    {
        throw std::runtime_error("Failed to get keyboard state.");
    }
    ToAscii(keypress.wVirtualKeyCode, keypress.wVirtualScanCode, keyboard_state, reinterpret_cast<LPWORD>(keybuf), 0);
    get_dispatcher()->c_new_message(keybuf[0], BUFFER_MSG_CODE);
}

bool cursor_movement_related_keys::process_keypress(KEY_EVENT_RECORD keypress)
{
    if (keypress.bKeyDown == 0)
    {
        return false;
    }
    if ((keypress.wVirtualKeyCode >= KEY_PRESS_LEFT_ARROW && keypress.wVirtualKeyCode <= KEY_PRESS_DOWN_ARROW) ||
        keypress.wVirtualKeyCode == KEY_PRESS_ENTER)
    {
        call_functions(keypress);
        return true;
    }
    return false;
}

void cursor_movement_related_keys::call_functions(const KEY_EVENT_RECORD& keypress)
{
    switch (keypress.wVirtualKeyCode)
    {
        case KEY_PRESS_LEFT_ARROW:
            for (int i = 0; i <= min(1, keypress.wRepeatCount/2); i++)
            {
                get_dispatcher()->s_new_message(LEFT_ARROW, CURSOR_MOVEMENT_LISTENER);
            }
            break;
        case KEY_PRESS_RIGHT_ARROW:
            for (int i = 0; i <= min(1, keypress.wRepeatCount/2); i++)
            {
                get_dispatcher()->s_new_message(RIGHT_ARROW, CURSOR_MOVEMENT_LISTENER);
            }
            break;
        case KEY_PRESS_UP_ARROW:
            for (int i = 0; i <= min(1, keypress.wRepeatCount/2); i++)
            {
                get_dispatcher()->s_new_message(UP_ARROW, CURSOR_MOVEMENT_LISTENER);
            }
            break;
        case KEY_PRESS_DOWN_ARROW:
            for (int i = 0; i <= min(1, keypress.wRepeatCount/2); i++)
            {
                get_dispatcher()->s_new_message(DOWN_ARROW, CURSOR_MOVEMENT_LISTENER);
            }
            break;
        case KEY_PRESS_ENTER:
            get_dispatcher()->s_new_message(RETURN_PRESS, CURSOR_MOVEMENT_LISTENER);
            break;
        default:
            throw std::runtime_error("Bad input for arrow handler");
    }
}
