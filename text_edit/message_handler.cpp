#include "message_handler.h"
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
void message_handler::c_new_message(char p, int x, int y, message_tags target)
{
    if (target == message_tags::BUFFER_MSG_CODE && !flags[static_cast<int>(program_flags::BUFFER_RUNNING)])
    {
        return;
    }
    cmessages.push(std::make_pair(std::make_pair(p, std::make_pair(x, y)), target));
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
std::pair<char, std::pair<int, int>> message_handler::c_pop_latest_msg_or_return_0(message_tags who)
{
    if (cmessages.empty())
    {
        return std::pair{'\0',std::pair{-1,-1}};
    }
    const std::pair<std::pair<char, std::pair<int, int>>, message_tags> c = cmessages.back();
    if (c.second == who)
    {
        cmessages.pop();
        return c.first;
    }
    return std::pair{'\0',std::pair{-1,-1}};
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
void message_handler::s_new_message(short i, message_tags who)
{
    _squeuelen++;
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
short message_handler::s_pop_latest_message_or_return_0(message_tags who)
{
    if (smessages.empty())
    {
        return 0;
    }
    const std::pair<short, message_tags> last = smessages.back();
    if (last.second == who)
    {
        _squeuelen--;
        smessages.pop();
        return last.first;
    }
    return 0;
}
void message_handler::set_flag(program_flags flag)
{
    if (flags[static_cast<int>(flag)])
    {
        return;
    }
    flags[static_cast<int>(flag)] = true;
}

bool message_handler::get_flag(program_flags flag)
{
    return flags[static_cast<int>(flag)];
}

void message_handler::clear_flag(program_flags flag)
{
    if (!flags[static_cast<int>(flag)])
    {
        return;
    }
    flags[static_cast<int>(flag)] = false;
}
std::pair<int, int> message_handler::get_cursor()
{
    return cursorstate;
}
void message_handler::set_cursor(int cursor_x, int cursor_y)
{
    cursorstate.first = cursor_x;
    cursorstate.second = cursor_y;
}
int message_handler::squeuelen()
{
    return _squeuelen;
}
