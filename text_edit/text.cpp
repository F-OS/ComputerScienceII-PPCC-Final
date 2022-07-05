#include "text.hpp"

#include <sstream>

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
        int center_x = (window.X - tmp.size()) / 2;
        newstr += std::string(center_x, ' ') + tmp + '\n';
        i++;
    }
    const int center_y = (window.Y - i) / 2;
    newstr = std::string(center_y, '\n') + newstr + '\n';
    return newstr;
}
void text::load_string(const std::string& string)
{
    std::stringstream ss(string);
    std::string tmp;
    while (std::getline(ss, tmp, '\n'))
    {
        if (tmp.length() == 0)
        {
            string_data tmp2;
            tmp2.line = "";
            const std::vector<std::string> blank{""};
            tmp2.displayable_substring = blank;
            string_objs.push_back(tmp2);
            continue;
        }
        string_data tmp2;
        tmp2.line = tmp;
        for (int i = 0; i < tmp2.line.length(); i += window.X - 1)
        {
            tmp2.displayable_substring.push_back(tmp2.line.substr(i, window.X - 1));
        }
        string_objs.push_back(tmp2);
    }
}
void text::set_cursor_pos(int x_pos, int y_pos)
{
    x_cursor_pos = x_pos;
    y_cursor_pos = y_pos;
    load_attributes();
    display_whole_buffer();
}
SHORT text::get_length_at(int y_loc)
{
    return string_objs.size() <= max(y_loc - 1, 0) ? 0 : string_objs[y_loc].displayable_substring[0].length();
}
void text::do_scroll(int i)
{
    currentoffset += i;
    currentoffset = max(0, min(i, string_objs.size() - 1));
}

void text::blit_to_screen_from_internal_buffer()
{
    std::string full;
    for (int i = currentoffset; i < window.Y - 1; i++)
    {
        if (i >= string_objs.size())
        {
            break;
        }
        std::string tmp = string_objs[i].displayable_substring[0];
        tmp += std::string(window.X - string_objs[i].displayable_substring[0].length(), ' ');
        full.append(tmp);
    }
    print_to_whole_console(full);
}

void text::load_attributes() const
{
    for (int array_pos = 0; array_pos < window.X * window.Y; array_pos++)
    {
        int array_x = array_pos % window.X;
        int array_y = floor(array_pos / window.X);
        if (x_cursor_pos == array_x && y_cursor_pos == array_y)
        {
            print_buf[array_pos].Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY | BACKGROUND_RED
                | FOREGROUND_INTENSITY;
        }
        else
        {
            print_buf[array_pos].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY |
                FOREGROUND_RED;
        }
    }
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
void text::print_to_whole_console(const std::string& basic_string)
{
    const auto c_str = basic_string.c_str();
    if (basic_string.length() > window.X * window.Y)
    {
        throw std::out_of_range("String longer than allowed window size passed to print to whole console.");
    }
    for (int array_pos = 0; array_pos < window.X * window.Y; array_pos++)
    {
        if (array_pos < basic_string.length())
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
