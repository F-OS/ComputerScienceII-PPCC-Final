#include "input_tracker.hpp"
#include "windowsapi.hpp"
input_tracker::input_tracker(dispatch& dispatch_pass) : dispatcher(&dispatch_pass)
{
    tracker_thread = std::thread(&input_tracker::Iinput_tracker_thread, this);
}

input_tracker::~input_tracker()
{
	killflag = true;
    tracker_thread.join();
}

template <typename _Tp, typename _Alloc>
inline bool operator==(
    const std::vector<_Tp, _Alloc>& __lhs,
    const std::vector<_Tp, _Alloc>& __rhs
    )
{
    return __lhs._M_base() == __rhs._M_base();
}

void input_tracker::Iinput_tracker_thread() const
{
	std::vector<KEY_EVENT_RECORD> keybuffer;
	std::vector<WINDOW_BUFFER_SIZE_RECORD> windowbuffer;
    dispatcher->get_windows_api()->save_console_mode();
    dispatcher->get_windows_api()->set_console_mode(1, ENABLE_WINDOW_INPUT);
	unsigned long bufferlength = 0;
	while (true)
	{
		if (killflag)
		{
			break;
		}
		if (dispatcher->get_lock_on_key_state())
		{
			continue;
		}
        if (dispatcher->get_windows_api()->console_has_input_buffered())
		{
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
			Sleep(17);
		}
	}
}
