#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H
#include <queue>


enum class message_tags { BUFFER_MSG_CODE, CURSOR_MOVEMENT_LISTENER };


enum class program_flags { CTRL_Q_PRESSED, KEYBOARD_OP_LOCK, CURSOR_ALTERED, MAXVAL };


class message_handler
{
public:
    void c_new_message(char p, message_tags target);

    char c_pop_latest_msg_or_return_0(message_tags who);

    void s_new_message(short i, message_tags who);

    short s_pop_latest_message_or_return_0(message_tags who);

    void set_flag(program_flags flag);

    bool get_flag(program_flags flag);

    void clear_flag(program_flags flag);

private:
    std::queue<std::pair<char, message_tags>> cmessages;
    std::queue<std::pair<short, message_tags>> smessages;
    bool flags[static_cast<int>(program_flags::MAXVAL) + 1] = {false,false};
};


extern message_handler global_message_handler;
#endif
