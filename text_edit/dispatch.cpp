#include "dispatch.hpp"

#include "input_tracker.hpp"
#include "menu.hpp"
#include "text.hpp"
#include "windowsapi.hpp"
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

text* dispatch::get_text_obj()
{
    return text_obj;
}

windowsapi* dispatch::get_windows_api()
{
    return windowsapihandle;
}
bool dispatch::menu_in_control()
{
    return false;
}
void dispatch::spawn_menu()
{
    main_menu newmenu(*this);
    newmenu.displaymenu();
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
    in = new input_tracker(*this);
    windowsapihandle = new windowsapi;
    text_obj = new text(*this);
    hooked_keypresses.push_back(std::make_shared<ctrl_q>(*this));
	hooked_keypresses.push_back(std::make_shared<ctrl_c>(*this));
	hooked_keypresses.push_back(std::make_shared<menu_hooked_keys>(*this));
	hooked_keypresses.push_back(std::make_shared<forward_to_buffer>(*this));
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
