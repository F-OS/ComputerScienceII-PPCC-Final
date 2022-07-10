#ifndef DISPATCH_H
#define DISPATCH_H
#include <memory>
#include <queue>

#include "cursor.h"
#include "key_press_handler.hpp"
#include "mainhead.h"
enum class message_tags;
class cursor;
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

    int spawn_menu();

    file_handler* get_file_obj();

    cursor* get_cursor_obj();

    void return_to_main();

    bool is_return_to_main();

    // Constructor & Destructor
    dispatch();

    // pseudomutex functions
    bool get_lock_on_key_state() const;

    void lock_key_state();

    void unlock_key_state();

    void send_to_key_handler();

    // Functions for terminating the program
    void exit_program();

    // Pure Getters and setters
    bool is_shutting_down() const;

private:
    std::queue<std::pair<char, message_tags>> cmessages;
    std::queue<std::pair<short, message_tags>> smessages;
    bool start_cleanup = false;
    bool reading_key_state = false;
    std::unique_ptr<text*> text_obj;
    std::unique_ptr<windowsapi*> windowsapihandle;
    std::unique_ptr<file_handler*> file_handle;
    std::unique_ptr<input_tracker*> in;
    std::unique_ptr<cursor*> cursorobj;
    bool retmain = false;
};
#endif
