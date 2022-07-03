#include "windowsapi.hpp"

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
const HANDLE& windowsapi::request_io_handle(int stream)
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
void windowsapi::save_console_mode()
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
void windowsapi::load_old_console_mode()
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
void windowsapi::set_console_mode(int stream, DWORD bitflags)
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
bool windowsapi::console_has_input_buffered()
{
	unsigned long number_of_events = 0;
    GetNumberOfConsoleInputEvents(input_handle, &number_of_events);
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
INPUT_RECORD* windowsapi::get_console_input_array(unsigned long& buffer_length)
{
	auto* const stream_handle = request_io_handle(1);
	auto* const buffer = new INPUT_RECORD[BUFSIZ];
	ReadConsoleInput(stream_handle, buffer, BUFSIZ, &buffer_length);
	return buffer;
}

COORD windowsapi::get_window_size()
{
    update_screen_buffer();
    GetConsoleScreenBufferInfo(output_handle, cbsi);
    return cbsi->dwMaximumWindowSize;
}
void windowsapi::update_screen_buffer()
{
    if (!input_handle || !output_handle || !error_handle || !cbsi)
    {
        request_io_handle(0);
        request_io_handle(1);
        request_io_handle(2);
        cbsi = new CONSOLE_SCREEN_BUFFER_INFO;
    }
    GetConsoleScreenBufferInfo(output_handle, cbsi);
}

CONSOLE_SCREEN_BUFFER_INFO windowsapi::get_cbsi()
{
    update_screen_buffer();
    return *cbsi;
}

COORD windowsapi::get_cursor()
{
    update_screen_buffer();
    return cbsi->dwCursorPosition;
}

COORD windowsapi::set_cursor(int x, int y)
{
    update_screen_buffer();
    COORD oldcursor = cbsi->dwCursorPosition;
    SetConsoleCursorPosition(request_io_handle(0), COORD{static_cast<short>(x),static_cast<short>(y)});
    return oldcursor;
}

