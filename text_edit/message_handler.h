#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H
#include <queue>

#include "cursor.h"


enum class message_tags { BUFFER_MSG_CODE, CURSOR_MOVEMENT_LISTENER };


enum class program_flags { CTRL_Q_PRESSED, KEYBOARD_OP_LOCK, CURSOR_ALTERED, BUFFER_RUNNING, MAXVAL };


class message_handler
{
public:
    void c_new_message(char p, int x, int y, message_tags target);

    std::pair<char, std::pair<int, int>> c_pop_latest_msg_or_return_0(message_tags who);

    void s_new_message(short i, message_tags who);

    short s_pop_latest_message_or_return_0(message_tags who);

    void set_flag(program_flags flag);

    bool get_flag(program_flags flag);

    void clear_flag(program_flags flag);

    std::pair<int, int> get_cursor();

    void set_cursor(int cursor_x, int cursor_y);

    int squeuelen();

private:
    std::queue<std::pair<std::pair<char, std::pair<int, int>>, message_tags>> cmessages;
    std::queue<std::pair<short, message_tags>> smessages;
    bool flags[static_cast<int>(program_flags::MAXVAL) + 1] = {false,false};
    std::pair<int, int> cursorstate;
    int _squeuelen = 0;
};


extern message_handler global_message_handler;
#endif
