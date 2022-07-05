#ifndef MAIN_HEAD_H
#define MAIN_HEAD_H
// System Includes
// Non-class defining program headers
#include <string>
#include <vector>

// Forward Declares
class windowsapi;
class dispatch;
class text;
class key_press_handler;
class file_handler;
class input_tracker;
class menu;


// Non-Class objects
struct string_data
{
    // This is our string from newline to newline.
    std::string line;
    // This is a vector of strings for display
    std::vector<std::string> displayable_substring; 
};


enum class message_tags { BUFFER_MSG_CODE, CURSOR_MOVEMENT_LISTENER };

#endif
