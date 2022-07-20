#include "text.hpp"

#include <sstream>

#include "message_handler.h"
#include "windowsapi.hpp"

text::text(dispatch& dispatch_pass)
{
    dispatcher = &dispatch_pass;
    window = dispatcher->get_windows_api()->get_window_size();
    output_handle = dispatcher->get_windows_api()->request_io_handle(0);
    print_buf = new CHAR_INFO[window.X * window.Y];
}

std::string text::center_text(const std::string& str)
{
    std::stringstream ss(str);
    std::string tmp;
    std::string newstr;
    int i = 0;
    while (std::getline(ss, tmp, '\n'))
    {
        const int center_x = (window.X - tmp.size()) / 2;
        newstr += std::string(center_x, ' ') + tmp + '\n';
        i++;
    }
    const int center_y = (window.Y - i) / 2;
    newstr = std::string(center_y, '\n') + newstr + '\n';
    return newstr;
}
void text::fill_buf()
{
    std::stringstream ss(bufstr);
    std::string tmp;
    string_objs.clear();
    while (std::getline(ss, tmp, '\n'))
    {
        if (tmp.length() == 0)
        {
            string_data tmp2;
            tmp2.line = "";
            const std::vector<std::string> blank{""};
            tmp2.displayable_substring = blank;
            string_objs.push_back(tmp2);
        }
        else
        {
            string_data tmp2;
            tmp2.line = tmp;
            for (int i = 0; i < tmp2.line.length(); i += window.X - 1)
            {
                tmp2.displayable_substring.push_back(tmp2.line.substr(i, window.X - 1));
            }
            string_objs.push_back(tmp2);
        }
    }
}
void text::load_buffer(const std::string& instr)
{
    bufstr = instr;
}

const std::string& text::read_buffer()
{
    return bufstr;
}

int text::get_length_at(int y_loc)
{
    lock_for_get_length = true;
    if (y_loc >= table.size() || y_loc < 0)
    {
        lock_for_get_length = false;
        return 0;
    }
    const int table_ret = table[y_loc];
    lock_for_get_length = false;
    return table_ret;
}

void text::do_scroll(int i)
{
    currentoffset += i;
    currentoffset = max(0, currentoffset);
    currentoffset = min(currentoffset, string_objs.size() - 1);
}

void text::blit_to_screen_from_internal_buffer()
{
    std::string full;
    bool buffermodified = false;
    int cursordelta = 0;
    global_message_handler.set_flag(program_flags::BUFFER_RUNNING);
    auto a = global_message_handler.c_pop_latest_msg_or_return_0(message_tags::BUFFER_MSG_CODE);
    while (a.second.second != -1 || a.second.first != -1)
    {
        if (a.first == '\b')
        {
            bufstr.erase(offset - 1, 1);
            cursordelta = -1;
        }
        else
        {
            cursordelta = 1;
            bufstr.reserve(bufstr.size() + 1);
            bufstr.insert(offset, 1, a.first);
        }
        a = global_message_handler.c_pop_latest_msg_or_return_0(message_tags::BUFFER_MSG_CODE);
        global_message_handler.set_flag(program_flags::CURSOR_ALTERED);
        buffermodified = true;
    }
    fill_buf();
    generate_length_table();
    for (int i = currentoffset; i < window.Y + currentoffset - 1; i++)
    {
        if (i >= string_objs.size())
        {
            break;
        }
        std::string tmp = string_objs[i].displayable_substring[0];
        tmp += std::string(window.X - string_objs[i].displayable_substring[0].length(), ' ');
        full.append(tmp);
    }
    if (global_message_handler.get_flag(program_flags::CURSOR_ALTERED))
    {
        const std::pair<int, int> curs = global_message_handler.get_cursor();
        x_cursor_pos = curs.first;
        y_cursor_pos = curs.second;
        dispatcher->get_windows_api()->set_cursor(curs.first, curs.second);
        global_message_handler.clear_flag(program_flags::CURSOR_ALTERED);
    }
    generate_length_table();
    if (buffermodified)
    {
        dispatcher->get_cursor_obj()->change_cursor(cursordelta, 0);
    }
    load_attributes();
    print_to_whole_console(full);
    Sleep(15);
}

void text::load_attributes()
{
    offset = 0;
    cursoroffset = 0;
    for (int i = 0; i < y_cursor_pos; i++)
    {
        cursoroffset += window.X;
    }
    for (int i = 0; i < y_cursor_pos; i++)
    {
        offset += get_length_at(i) - 1;
    }
    offset += x_cursor_pos + y_cursor_pos * 2;
    cursoroffset += x_cursor_pos;
    for (int array_pos = 0; array_pos < window.X * window.Y; array_pos++)
    {
        print_buf[array_pos].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY |
            FOREGROUND_RED;
    }
    print_buf[cursoroffset].Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY | BACKGROUND_RED |
        FOREGROUND_INTENSITY;
}

void text::display_whole_buffer()
{
    SMALL_RECT output_rect = {
        static_cast<short>(0),
        static_cast<short>(0),
        (window.X),
        (window.Y)
    };
    WriteConsoleOutputA(
                        output_handle,
                        print_buf,
                        window,
                        {0,0},
                        &output_rect
                       );
}
void text::print_to_whole_console(const std::string& instr)
{
    const auto c_str = instr.c_str();
    if (instr.length() > window.X * window.Y)
    {
        throw std::out_of_range("String longer than allowed window size passed to print to whole console.");
    }
    for (int array_pos = 0; array_pos < window.X * window.Y; array_pos++)
    {
        if (array_pos < instr.length())
        {
            print_buf[array_pos].Char.AsciiChar = c_str[array_pos];
        }
        else
        {
            print_buf[array_pos].Char.AsciiChar = '\0';
        }
    }
    load_attributes();
    display_whole_buffer();
}

void text::generate_length_table()
{
    if (lock_for_get_length)
    {
        return;
    }
    table.clear();
    table.reserve(string_objs.size());
    for (int i = 0; i < string_objs.size(); i++)
    {
        table.push_back(string_objs[i].line.length());
    }
}
