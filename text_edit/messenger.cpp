#include "messenger.hpp"

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
void messenger::c_new_message(char p, message_tags target)
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
char messenger::c_pop_latest_msg_or_return_0(message_tags who)
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
void messenger::s_new_message(short i, message_tags who)
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
short messenger::s_pop_latest_message_or_return_0(message_tags who)
{
    if (smessages.empty())
    {
        return 0;
    }
    const std::pair<short, message_tags> last = smessages.back();
    if (last.second == who)
    {
        smessages.pop();
        return last.first;
    }
    return 0;
}
