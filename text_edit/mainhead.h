#ifndef MAIN_HEAD_H
#define MAIN_HEAD_H
// System Includes
#include "Windows.h"
// Non-class defining program headers
#include <string>
#include <vector>

#include "Keytable.hpp"
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
    std::string line;                               // This is our string from newline to newline.
    std::vector<std::string> displayable_substring; // This is a vector of strings for display
};


enum message_tags { BUFFER_MSG_CODE, CURSOR_MOVEMENT_LISTENER };

#endif
