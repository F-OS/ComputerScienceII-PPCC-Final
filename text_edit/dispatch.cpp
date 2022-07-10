#include "dispatch.hpp"

#include <stdexcept>

#include "cursor.h"
#include "file_handler.hpp"
#include "input_tracker.hpp"
#include "menu.hpp"
#include "text.hpp"
#include "windowsapi.hpp"

#include "message_handler.h"

/*
 * Function: is_shutting_down
 * Function Purpose: Getter of start_cleanup
 * Return:
 *  - bool:  start_cleanup
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

text* dispatch::get_text_obj()
{
    return *text_obj;
}

windowsapi* dispatch::get_windows_api()
{
    return *windowsapihandle;
}
bool dispatch::menu_in_control()
{
    return false;
}

file_handler* dispatch::get_file_obj()
{
    return *file_handle;
}
cursor* dispatch::get_cursor_obj()
{
    return *cursorobj;
}
void dispatch::return_to_main()
{
    retmain = true;
}
bool dispatch::is_return_to_main()
{
    if (retmain)
    {
        retmain = false;
        return true;
    }
    return false;
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
    file_handle = std::make_unique<file_handler*>(new file_handler(""));
    windowsapihandle = std::make_unique<windowsapi*>(new windowsapi(*this));
    cursorobj = std::make_unique<cursor*>(new cursor(*this));
    in = std::make_unique<input_tracker*>(new input_tracker);
    text_obj = std::make_unique<text*>(new text(*this));
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

void dispatch::exit_program()
{
    start_cleanup = true;
}
