#include "input_tracker.hpp"
#include "windowsapi.hpp"
input_tracker::input_tracker(dispatch& dispatch_pass) : dispatcher(&dispatch_pass)
{
    tracker_thread = std::thread(&input_tracker::input_tracker_thread, this);
}

input_tracker::~input_tracker()
{
    tracker_thread.join();
}

void input_tracker::input_tracker_thread() const
{
    std::vector<KEY_EVENT_RECORD> keybuffer;
    std::vector<WINDOW_BUFFER_SIZE_RECORD> windowbuffer;

    unsigned long bufferlength = 0;
    while (true)
    {
        if (dispatcher->is_shutting_down())
        {
            break;
        }
        if (dispatcher->get_lock_on_key_state())
        {
            continue;
        }
        if (!dispatcher->get_windows_api()->console_has_input_buffered())
        {
            continue;
        }
        dispatcher->lock_key_state();
        const INPUT_RECORD* inputbuf = dispatcher->get_windows_api()->get_console_input_array(bufferlength);
        for (size_t i = 0; i < bufferlength; i++)
        {
            switch (inputbuf[i].EventType)
            {
                case KEY_EVENT:
                    keybuffer.push_back(inputbuf[i].Event.KeyEvent);
                    break;
                case WINDOW_BUFFER_SIZE_EVENT:
                    windowbuffer.push_back(inputbuf[i].Event.WindowBufferSizeEvent);
                    break;
                default:
                    break;
            }
        }
        delete[] inputbuf;
        dispatcher->send_to_key_handler(keybuffer);
        keybuffer.clear();
        dispatcher->unlock_key_state();
        Sleep(20);
    }
}
