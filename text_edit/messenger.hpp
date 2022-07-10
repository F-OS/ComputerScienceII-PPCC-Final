#pragma once
#include <queue>

#include "cursor.h"
#include "mainhead.h"


class messenger
{
public:
    void c_new_message(char p, message_tags target);

    char c_pop_latest_msg_or_return_0(message_tags who);

    void s_new_message(short i, message_tags who);

    short s_pop_latest_message_or_return_0(message_tags who);

private:
    std::queue<std::pair<char, message_tags>> cmessages;
    std::queue<std::pair<short, message_tags>> smessages;
};
