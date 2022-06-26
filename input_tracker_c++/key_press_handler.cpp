// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "key_press_handler.h"

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
		if (req_alt_key && (((keypress.dwControlKeyState & LEFT_ALT_PRESSED) != 0u) || ((keypress.dwControlKeyState &
			RIGHT_ALT_PRESSED) != 0u)))
		{
			call_functions();
			return true;
		}
		if (req_ctrl_key && (((keypress.dwControlKeyState & LEFT_CTRL_PRESSED) != 0u) || ((keypress.dwControlKeyState &
			RIGHT_CTRL_PRESSED) != 0u)))
		{
			call_functions();
			return true;
		}
	}
	return false;
}

void ctrl_q::call_functions()
{
	std::cout << "Goodbye!" << std::endl;
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
		if (((keypress.dwControlKeyState & LEFT_ALT_PRESSED) != 0u) || ((keypress.dwControlKeyState & RIGHT_ALT_PRESSED)
			!= 0u) || ((keypress.
			dwControlKeyState & LEFT_CTRL_PRESSED) != 0u) || ((keypress.dwControlKeyState & RIGHT_CTRL_PRESSED) != 0u))
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
	if(!GetKeyboardState(keyboard_state))
	{
		throw std::runtime_error("Failed to get keyboard state.");
	}
	ToAscii(keypress.wVirtualKeyCode, keypress.wVirtualScanCode, keyboard_state, reinterpret_cast<LPWORD>(keybuf), 0);
	get_dispatcher()->c_new_message(keybuf[0], BUFFER_MSG_CODE);
}

bool menu_hooked_keys::process_keypress(KEY_EVENT_RECORD keypress)
{
	if (keypress.bKeyDown != 0)
	{
		return false;
	}
	if ((keypress.wVirtualKeyCode >= KEY_PRESS_LEFT_ARROW && keypress.wVirtualKeyCode <= KEY_PRESS_DOWN_ARROW) || keypress
		.wVirtualKeyCode == KEY_PRESS_ENTER)
	{
		call_functions(keypress);
		return true;
	}
	return false;
}

void menu_hooked_keys::call_functions(const KEY_EVENT_RECORD& keypress)
{
	switch (keypress.wVirtualKeyCode)
	{
	case KEY_PRESS_LEFT_ARROW:
		get_dispatcher()->menu_pass_key(LEFT_ARROW);
		break;
	case KEY_PRESS_RIGHT_ARROW:
		get_dispatcher()->menu_pass_key(RIGHT_ARROW);
		break;
	case KEY_PRESS_UP_ARROW:
		get_dispatcher()->menu_pass_key(UP_ARROW);
		break;
	case KEY_PRESS_DOWN_ARROW:
		get_dispatcher()->menu_pass_key(DOWN_ARROW);
		break;
	case KEY_PRESS_ENTER:
		get_dispatcher()->menu_pass_key(RETURN_PRESS);
		break;
	default:
		throw std::runtime_error("Bad input for menu handler");
	}
}
