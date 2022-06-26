#ifndef DISPATCH_H
#define DISPATCH_H
#include <memory>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include "Windows.h"
#include "key_press_handler.hpp"

class text_render;
class key_press_handler;

enum message_tags { BUFFER_MSG_CODE, MENU_MSG_CODE };

/*
 * Class: dispatch
 * Class Purpose: This class allows asynchronous communication between threads and provides an interface between the Windows API and program code.
 * Constructor Arguments:
 *	None.
 * Example:
 *		int main()
 *		{
 *		dispatch d;
 *		auto t = new std::thread(a, std::ref(d));
 *		while(1)
 *		{
 *			std::cout << d.c_pop_latest_msg_or_return_0(0) << std::endl;
 *		}
 *		}
 *		void a(dispatch& d)
 *		{
 *			while(1) {d.c_new_message('a', 0);}
 *		}
 * Considerations:
 *	Dispatch cannot be copied or moved.
 *	Only one instance of dispatch should be created.
 *  Calling get_console_input_array without securing a key_state lock while InputTracker is initialized will create an inconsistent program state.
 */
class dispatch
{
public:
	// Overridden copy and move functions. Dispatch should never be copied or moved due to the risk of creating an inconsistent state.
	dispatch(dispatch&& other) = delete;
	dispatch(const dispatch& other) = delete;
	dispatch& operator=(dispatch&& other) = delete;
	dispatch& operator=(const dispatch&& other) = delete;
	text_render* get_text_render();
	void update_screen_buffer();
	CONSOLE_SCREEN_BUFFER_INFO get_cbsi();
	COORD set_cursor(HANDLE& sb, int x, int y);
	HANDLE open_screen_buf();
	void close_screen_buf();

	// Constructor & Destructor
	dispatch();
	~dispatch();
	// Windows console interfaces
	const HANDLE& request_io_handle(int stream);
	void save_console_mode();
	void load_old_console_mode();
	void set_console_mode(int stream, DWORD bitflags);
	bool console_has_input_buffered();
	INPUT_RECORD* get_console_input_array(unsigned long& buffer_length);
	COORD get_window_size();
	// pseudomutex functions
	bool get_lock_on_key_state() const;
	void lock_key_state();
	void unlock_key_state();
	// Messaging functions
	void send_to_key_handler(const std::vector<_KEY_EVENT_RECORD>& keypresses) const;
	void c_new_message(char p, message_tags target);
	char c_pop_latest_msg_or_return_0(message_tags who);
	void s_new_message(short i, message_tags who);
	short s_pop_latest_message_or_return_0(message_tags who);
	// Functions for terminating the program
	void exit_program();
	static void force_kill();
	// Pure Getters and setters
	bool is_shutting_down() const;

private:
	std::queue<std::pair<char, message_tags>> cmessages;
	std::queue<std::pair<short, message_tags>> smessages;
	bool start_cleanup = false;
	bool reading_key_state = false;
	HANDLE error_handle{};
	HANDLE output_handle{};
	HANDLE input_handle{};
	DWORD old_console_mode_stdin = 0;
	DWORD old_console_mode_stdio = 0;
	DWORD old_console_mode_stderr = 0;
	std::vector<std::shared_ptr<key_press_handler>> hooked_keypresses;
	std::thread* _kill;
	std::shared_ptr<text_render*> text_render_obj;
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_cbsi;
	HANDLE current_screen_buffer = nullptr;
};
#endif
