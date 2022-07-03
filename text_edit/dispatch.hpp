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

class windowsapi;
class text;
class key_press_handler;

class input_tracker;

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
    // Overridden copy and move functions. Dispatch should never be copied or moved due to the risk of creating an inconsistent state. (Two text_render objects trying to access the same stdio, etc)
	dispatch(dispatch&& other) = delete;
	dispatch(const dispatch& other) = delete;
	dispatch& operator=(dispatch&& other) = delete;
	dispatch& operator=(const dispatch&& other) = delete;

    // Getters for various external objects.
    text* get_text_obj();

    windowsapi* get_windows_api();

    bool menu_in_control();

    void spawn_menu();

    // Constructor & Destructor
	dispatch();
	~dispatch();

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
    text* text_obj;
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_cbsi;
	HANDLE current_screen_buffer = nullptr;
    windowsapi* windowsapihandle;
    input_tracker* in;
};
#endif
