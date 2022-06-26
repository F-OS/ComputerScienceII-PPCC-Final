


#include "input_tracker.h"

input_tracker::input_tracker(dispatch& dispatch_pass) : dispatcher(&dispatch_pass)
{
	tracker_thread = std::make_unique<std::thread>(std::thread(&input_tracker::Iinput_tracker_thread, this));
}

input_tracker::~input_tracker()
{
	killflag = true;
	(*tracker_thread).join();
}

void input_tracker::Iinput_tracker_thread() const
{
	std::vector<KEY_EVENT_RECORD> keybuffer;
	std::vector<WINDOW_BUFFER_SIZE_RECORD> windowbuffer;
	dispatcher->save_console_mode();
	dispatcher->set_console_mode(1, ENABLE_WINDOW_INPUT);
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
		if (dispatcher->console_has_input_buffered())
		{
			dispatcher->lock_key_state();
			const INPUT_RECORD* inputbuf = dispatcher->get_console_input_array(bufferlength);
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
