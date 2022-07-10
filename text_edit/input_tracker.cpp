#include "input_tracker.hpp"

#include "key_press_handler.hpp"
#include "message_handler.h"
#include "windowsapi.hpp"

input_tracker::input_tracker()
{
    tracker_thread = std::thread(&input_tracker::input_tracker_thread, this);
    hooked_keypresses.push_back(std::make_shared<ctrl_q>());
    //hooked_keypresses.push_back(std::make_shared<ctrl_c>(*this)); TODO implement ctrl-c
    hooked_keypresses.push_back(std::make_shared<cursor_movement_related_keys>());
    hooked_keypresses.push_back(std::make_shared<forward_to_buffer>());
}

const std::vector<KEY_EVENT_RECORD>& input_tracker::get_keybuffer() const
{
    return keybuffer;
}
const std::vector<WINDOW_BUFFER_SIZE_RECORD>& input_tracker::get_windowbuffer() const
{
    return windowbuffer;
}
bool input_tracker::get_thread_lock() const
{
    return keyslocked;
}

void input_tracker::set_thread_lock()
{
    if (keyslocked)
    {
        return;
    }
    keyslocked = true;
}

void input_tracker::clear_thread_lock()
{
    if (!keyslocked)
    {
        return;
    }
    keyslocked = false;
}

input_tracker::~input_tracker()
{
    kill_thread();
}

void input_tracker::kill_thread()
{
    if (!endthread)
    {
        endthread = true;
        tracker_thread.join();
    }
    endthread = false;
}
void input_tracker::input_tracker_thread()
{
    active = true;
    unsigned long number_of_events = 0;
    unsigned long bufferlength = 0;
    while (true)
    {
        if (endthread)
        {
            break;
        }
        if (global_message_handler.get_flag(program_flags::KEYBOARD_OP_LOCK))
        {
            continue;
        }
        GetNumberOfConsoleInputEvents(windowsapi::request_io_handle(1), &number_of_events);
        if (!number_of_events)
        {
            continue;
        }
        global_message_handler.set_flag(program_flags::KEYBOARD_OP_LOCK);
        const INPUT_RECORD* inputbuf = windowsapi::get_console_input_array(bufferlength);
        for (size_t i = 0; i < bufferlength; i++)
        {
            switch (inputbuf[i].EventType)
            {
                case KEY_EVENT:
                    if (inputbuf[i].Event.KeyEvent.wRepeatCount > 1)
                    {
                        for (int i = 0; i < inputbuf[i].Event.KeyEvent.wRepeatCount; i++)
                        {
                            keybuffer.push_back(inputbuf[i].Event.KeyEvent);
                        }
                    }
                    else
                    {
                        keybuffer.push_back(inputbuf[i].Event.KeyEvent);
                    }
                    break;
                case WINDOW_BUFFER_SIZE_EVENT:
                    windowbuffer.push_back(inputbuf[i].Event.WindowBufferSizeEvent);
                    break;
                default:
                    break;
            }
        }
        delete[] inputbuf;
        if (!keybuffer.empty())
        {
            send_to_key_handler();
        }
        global_message_handler.clear_flag(program_flags::KEYBOARD_OP_LOCK);
        Sleep(20);
    }
    active = false;
}

void input_tracker::send_to_key_handler()
{
    const auto keypresses = keybuffer;
    keybuffer.clear();
    for (const auto& key : keypresses)
    {
        for (const auto& kp : hooked_keypresses)
        {
            if (kp->process_keypress(key))
            {
                break;
            }
        }
    }
}
