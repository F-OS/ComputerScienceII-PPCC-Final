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
#include "key_press_handler.h"

enum message_tags {BUFFER_MSG_CODE, MENU_MSG_CODE};


class dispatch
{
public:
	~dispatch();
	dispatch(dispatch&& other) = delete;
	dispatch(const dispatch& other) = delete;
	dispatch& operator=(dispatch&& other) = delete;
	dispatch& operator=(const dispatch&& other) = delete;
	// Constructor
	dispatch();
	// Windows console interfaces
	const HANDLE& request_io_handle(int stream);
	void save_console_mode();
	void load_old_console_mode();
	void set_console_mode(int stream, DWORD bitflags);
	bool console_has_input_buffered();
	INPUT_RECORD* get_console_input_array(unsigned long& buffer_length);
	unsigned char* get_keyboard_state();
	// pseudomutex functions
	bool get_lock_on_key_state() const;
	void lock_key_state();
	void unlock_key_state();
	// Messaging functions
	void send_to_key_handler(const std::vector<_KEY_EVENT_RECORD>& keypresses) const;
	void c_new_message(char p, int target);
	char c_pop_latest_msg_or_return_0(int who);
	void menu_pass_key(short i);
	short menu_read_key();
	// Kill program
	void exit_program();
	static void force_kill();
	// Pure Getters and setters
	bool is_shutting_down() const;

private:
	std::queue<std::pair<char, int>> cmessages;
	bool start_cleanup = false;
	bool reading_key_state = false;
	HANDLE error_handle{};
	HANDLE output_handle{};
	HANDLE input_handle{};
	DWORD old_console_mode_stdin = 0;
	DWORD old_console_mode_stdio = 0;
	DWORD old_console_mode_stderr = 0;
	std::queue<int> last_menu_key;
	std::vector<std::shared_ptr<key_press_handler>> hooked_keypresses;
	std::thread* _kill;
};
#endif
