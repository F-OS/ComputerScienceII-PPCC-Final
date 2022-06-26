#include "dispatch.hpp"
#include "text_render.hpp"

/*
 * Function: exit_program
 * Function Purpose: Exits the program.
 * Arguments: No arguments.
 * Function Flow:
 *	This function sets a bool that calls the cleanup functions of every subsystem through a conditional in the main loop.
 *	The function then spawns a thread to kill the program if cleanup goes over time.
 * Return:
 *	No returns.
 * Throws:
 *	No throws.
 */
void dispatch::exit_program()
{
	start_cleanup = true;
	_kill = new std::thread(&dispatch::force_kill);
}

/*
 * Function: is_shutting_down
 * Function Purpose: Getter of start_cleanup
 * Return:
 *	 - bool:  start_cleanup
 */
bool dispatch::is_shutting_down() const
{
	return start_cleanup;
}

/*
 * Function: ~dispatch
 * Function Purpose: Destructor of dispatch
 * Arguments: No arguments
 * Function Flow:
 *	This function joins the kill thread for posterity.
 * Return:
 *	N/A, joining kill exits the program.
 * Throws:
 *	No throws.
 */
dispatch::~dispatch()
{
	_kill->join();
}

text_render* dispatch::get_text_render()
{
	return *text_render_obj.get();
}

/*
 * Function: dispatch
 * Function Purpose: Initializes dispatch
 * Arguments: No arguments
 * Function Flow:
 *	This function pushes back derived classes of key_press_handler as shared_ptrs.
 * Return: Constructor
 * Throws: No throws
 */
dispatch::dispatch()
{
	text_render* tr = new text_render(*this);
	text_render_obj = std::make_shared<text_render*>(tr);
	hooked_keypresses.push_back(std::make_shared<ctrl_q>(*this));
	hooked_keypresses.push_back(std::make_shared<ctrl_c>(*this));
	hooked_keypresses.push_back(std::make_shared<menu_hooked_keys>(*this));
	hooked_keypresses.push_back(std::make_shared<forward_to_buffer>(*this));
}

/*
 * Function: request_io_handle
 * Function Purpose: Return a handle to one of the standard windows console streams.
 * Arguments:
 *	stream: int - allows switching between streams:
 *		0 - return a handle to standard output.
 *		1 - return a handle to standard input.
 *		2 - return a handle to standard error.
 * Function Flow:
 *	This function switches based on the value of stream and checks if the requested handle exists.
 *	If not, it creates a new handle, stores it as an attribute, and returns it to the user.
 * Throws:
 *	out_of_range exception: throws if stream not between 0 and 2
 *	runtime_error: throws if windows refuses to allow it to lock the console's streams.
 */
const HANDLE& dispatch::request_io_handle(int stream)
{
	switch (stream)
	{
	case 0:
		if (output_handle == nullptr)
		{
			output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		}
		if (output_handle == INVALID_HANDLE_VALUE)
		{
			throw std::runtime_error(
				"Unable to lock console handle. Function request_io_handle. Provided arguments: " +
				std::to_string(stream) + " Error location: " + __FILE__ + ", " + std::to_string(__LINE__));
		}
		return output_handle;
	case 1:
		if (input_handle == nullptr)
		{
			input_handle = GetStdHandle(STD_INPUT_HANDLE);
		}
		if (input_handle == INVALID_HANDLE_VALUE)
		{
			throw std::runtime_error(
				"Unable to lock console handle. Function request_io_handle. Provided arguments: " +
				std::to_string(stream) + " Error location: " + __FILE__ + ", " + std::to_string(__LINE__));
		}
		return input_handle;
	case 2:
		if (error_handle == nullptr)
		{
			error_handle = GetStdHandle(STD_ERROR_HANDLE);
		}
		if (error_handle == INVALID_HANDLE_VALUE)
		{
			throw std::runtime_error(
				"Unable to lock console handle. Function request_io_handle. Provided arguments: " +
				std::to_string(stream) + " Error location: " + __FILE__ + ", " + std::to_string(__LINE__));
		}
		return error_handle;
	default:
		throw std::out_of_range(
			"This function requires an argument of int in range [0, 3]. Function request_io_handle. Provided arguments: "
			+ std::to_string(stream) + " Error location: " + __FILE__ + ", " + std::to_string(__LINE__));
	}
}

/*
 * Function: save_console_mode
 * Function Purpose: Saves the console mode before modification by the program.
 * Function Flow:
 *	This function first secures three handles to each of the three streams.
 *	It then uses the GetConsoleMode function to load those streams into the old_console_mode buffers.
 * Return:
 *	void function
 * Throws:
 *	No throws
 */
void dispatch::save_console_mode()
{
	auto* const stdio_mode = request_io_handle(0);
	auto* const stdin_mode = request_io_handle(1);
	auto* const stderr_mode = request_io_handle(2);
	GetConsoleMode(stdin_mode, &old_console_mode_stdio);
	GetConsoleMode(stdio_mode, &old_console_mode_stdin);
	GetConsoleMode(stderr_mode, &old_console_mode_stderr);
}

/*
 * Function: load_old_console_mode
 * Function Purpose: Loads previously saved console states.
 * Arguments: No arguments.
 * Function Flow:
 *	This function first secures three handles to standard input/output/error.
 *	It then uses the SetConsoleMode function to load previously saved buffers into standard input/output/error.
 * Return:
 *	Void function
 * Throws:
 *	No throws
 */
void dispatch::load_old_console_mode()
{
	const auto stdio_mode = request_io_handle(0);
	const auto stdin_mode = request_io_handle(1);
	const auto stderr_mode = request_io_handle(2);
	SetConsoleMode(stdin_mode, old_console_mode_stdio);
	SetConsoleMode(stdio_mode, old_console_mode_stdin);
	SetConsoleMode(stderr_mode, old_console_mode_stderr);
}

/*
 * Function: set_console_mode
 * Function Purpose: Sets bitflags on standard input/output/error
 * Arguments:
 *	 stream - int: The number of the stream, 0 is standard output, 1 is standard input, 2 is standard error.
 *	 bitflags - DWORD: 64 bit unsigned long holding a set of bitflags. Further documentation at https://docs.microsoft.com/en-us/windows/console/setconsolemode
 * Function Flow:
 * The function first establishes a handle to the requested stream.
 * The function then sets the flags provided in bitflags and exits.
 * Return:
 * Void function
 * Throws:
 * No throws
 */
void dispatch::set_console_mode(int stream, DWORD bitflags)
{
	auto* const stream_handle = request_io_handle(stream);
	SetConsoleMode(stream_handle, bitflags);
}

/*
 * Function: console_has_input_buffered
 * Function Purpose: Tells the user if the console has input that can be read
 * Arguments:
 *	None
 * Function Flow:
 *	First the function secures a handle to standard input.
 *	Then it calls GetNumberOfConsoleInputEvents and saves that function's output to number_of_events.
 *	Finally it returns a boolean variable to indicate whether number_of_events is non-zero.
 * Return:
 *	 - bool: Does the console have input buffered?
 * Throws:
 *	No throws.
 */
bool dispatch::console_has_input_buffered()
{
	unsigned long number_of_events = 0;
	const HANDLE stream_handle = request_io_handle(1);
	GetNumberOfConsoleInputEvents(stream_handle, &number_of_events);
	return number_of_events != 0;
}

/*
 * Function: get_console_input_array
 * Function Purpose: Return an array of INPUT_RECORDs to the user for further processing.
 * Arguments:
 *	 buffer_length - unsigned long&: a reference to a variable that this function modifies to contain the length of the array buffer.
 * Function Flow:
 *	First this function secures a handle to standard output and dynamically initializes a buffer.
 *	Then this function calls ReadConsoleInput and fills that buffer with any INPUT_RECORDS that were in the console's queue.
 *	Finally the function returns the buffer.
 * Return:
 *	 - INPUT_RECORD*: a pointer to the first member of the INPUT_RECORD array.
 *	 - unsigned long ref: The length of the INPUT_RECORD array.
 * Throws:
 * No throws.
 */
INPUT_RECORD* dispatch::get_console_input_array(unsigned long& buffer_length)
{
	auto* const stream_handle = request_io_handle(1);
	auto* const buffer = new INPUT_RECORD[BUFSIZ];
	ReadConsoleInput(stream_handle, buffer, BUFSIZ, &buffer_length);
	return buffer;
}

COORD dispatch::get_window_size()
{
	auto* const stream_handle = request_io_handle(0);
	return GetLargestConsoleWindowSize(stream_handle);
}

/*
 * Function: get_lock_on_key_state
 * Function Purpose: Return a lock on the keyboard state.
 * Arguments: None
 * Function Flow:
 *	this function returns the member field reading_key_state.
 * Return:
 *	 - bool: lock on keystate.
 * Throws:
 */
bool dispatch::get_lock_on_key_state() const
{
	return reading_key_state;
}

/*
 * Function: lock_key_state
 * Function Purpose: Locks the keyboard state from modification in threads other than the one that called this function.
 * Arguments: None
 * Function Flow:
 *	This function checks if a lock is already set, if so, it throws an exception as all functions calling lock_key_state should check for locks before calling.
 *	If not, the function sets a lock.
 * Return: Void function.
 * Throws:
 *	std::runtime_error - This funtion throws a runtime error if reading_key_state is set.
 */
void dispatch::lock_key_state()
{
	if (reading_key_state)
	{
		throw std::runtime_error("Inconsistent program state. reading_key_state locked while already locked.");
	}
	reading_key_state = true;
}

/*
 * Function: unlock_key_state
 * Function Purpose: Unlocks the keyboard state, allowing for modification.
 * Arguments: None
 * Function Flow:
 *	This function checks if lock is unset, if so, it throws an exception as all functions calling unlock_key_state should have locked the key_state beforehand.
 *	If not, the function resets the lock.
 * Return: Void function.
 * Throws:
 *	std::runtime_error - This function throws a runtime error if reading_key_state is unset.
 */
void dispatch::unlock_key_state()
{
	if (!reading_key_state)
	{
		throw std::runtime_error("Inconsistent program state. reading_key_state unlocked while already unlocked.");
	}
	reading_key_state = false;
}

/*
 * Function: send_to_key_handler
 * Function Purpose: This function takes a list of keypresses and finds a handler for each of them.
 * Arguments:
 *	 keypresses - const std::vector<_KEY_EVENT_RECORD>&: A vector containing Windows KEY_EVENT_RECORDS
 * Function Flow:
 *	This function loops through each key in the vector passed as an argument, then loops through each handler in hooked_keypresses and calls process_keypress.
 * Return:
 *	Void function.
 * Throws:
 *	std::runtime_error - This function throws a runtime error if a keypress is logged with no corresponding handler.
 */
void dispatch::send_to_key_handler(const std::vector<_KEY_EVENT_RECORD>& keypresses) const
{
	int handler_check = 0;
	for (const auto& key : keypresses)
	{
		for (const auto& kp : hooked_keypresses)
		{
			if (kp->process_keypress(key))
			{
				break;
			}
			else
			{
				handler_check++;
			}
			if (handler_check == hooked_keypresses.size() && key.bKeyDown != true)
			{
				throw std::runtime_error("Unhandled keypress. Keypress code:" + key.wVirtualScanCode);
			}
		}
	}
}

/*
 * Function: c_new_message
 * Function Purpose: This function sends a message to be asynchronously read by another program subsystem.
 * Arguments:
 *	 p - char: A character.
 *	 target - message_tags: An enum to associate the message with a specific subsystem. 0 is text_buffer, 1 is menu, etc.
 * Function Flow:
 *	This function pushes a pair consisting of a char and int into the std::queue object for char messages.
 * Return:
 *	No returns.
 * Throws:
 *	No throws.
 */
void dispatch::c_new_message(char p, message_tags target)
{
	cmessages.push(std::make_pair(p, target));
}

/*
 * Function: c_pop_latest_msg_or_return_0
 * Function Purpose: Receive a message asynchronously added to the queue.
 * Arguments:
 *	 who - message_tags: The id of the class calling this function.
 * Function Flow:
 *	First, this function checks if the queue is empty. If so, it returns the 0-length c-string '\0'.
 *	If not, it creates a pair from the last (most recent) item on the queue.
 *	It then checks if the second element in that pair (the tag) matches the one passed to the function.
 *	If so, it removes the item used to create the pair from the queue and returns the char.
 * Return:
 *	 - char: The message added to the queue or '\0' if the queue is empty.
 * Throws:
 *	No throws.
 */
char dispatch::c_pop_latest_msg_or_return_0(message_tags who)
{
	if (cmessages.empty())
	{
		return '\0';
	}
	const std::pair<char, message_tags> c = cmessages.back();
	if (c.second == who)
	{
		cmessages.pop();
		return c.first;
	}
	return '\0';
}

/*
 * Function: s_new_message
 * Function Purpose: c_new_message but for shorts.
 * Arguments:
 *	 i - short: a short to be added to the message queue.
 *   target - message_tags: An enum to associate the message with a specific subsystem. 0 is text_buffer, 1 is menu, etc.
 * Function Flow:
 *	This function pushes a pair consisting of a short and int into the std::queue object for short messages.
 * Return:
 *	Void function
 * Throws:
 *	No throws.
 */
void dispatch::s_new_message(short i, message_tags who)
{
	smessages.push(std::make_pair(i, who));
}

/*
 * Function: s_pop_latest_message_or_return_0
 * Function Purpose: Receive a message asynchronously added to the queue.
 * Arguments:
 *	 who - message_tags: The id of the class calling this function.
 * Function Flow:
 * 	First, this function checks if the queue is empty. If so, it returns the short 0.
 *	If not, it creates a pair from the last (most recent) item on the queue.
 *	It then checks if the second element in that pair (the tag) matches the one passed to the function.
 *	If so, it removes the item used to create the pair from the queue and returns the short.
 * Return:
 *	 - short: The message added to the queue or 0 if the queue is empty.
 * Throws:
 *	No throws.
 */
short dispatch::s_pop_latest_message_or_return_0(message_tags who)
{
	if (smessages.empty())
	{
		return 0;
	}
	std::pair<short, message_tags> last = smessages.back();
	if (last.second == who)
	{
		smessages.pop();
		return last.first;
	}
	return 0;
}

/*
 * Function: force_kill
 * Function Purpose: Exit the program.
 * Arguments: None
 * Function Flow:
 *	This function sleeps for 1 second to allow the file handler and text buffer to finish up their work.
 *	The function then calls exit() if it hasn't already been called by text_edit.
 * Return:
 *	N/A exits the program.
 * Throws:
 *	No throws.
 */
void dispatch::force_kill()
{
	Sleep(1000);
	std::cout << "Cleanup overtime. Exiting." << std::endl;
	exit(EXIT_SUCCESS);
}
