#include "text.hpp"

#include <sstream>

#include "windowsapi.hpp"

text::text(dispatch& dispatcher)
{
	this->dispatcher = &dispatcher;
    output_handle = this->dispatcher->get_windows_api()->request_io_handle(0);
    SMALL_RECT window_size{0,0,120,30};
    COORD buffer_size{120,30};
    SetConsoleTitleA("TextEdit");
    SetConsoleWindowInfo(output_handle, true, &window_size);
    SetConsoleScreenBufferSize(output_handle, buffer_size);
}

std::string text::center_text(const std::string& str)
{
    COORD window = dispatcher->get_windows_api()->get_window_size();
	std::stringstream ss(str);
	std::string tmp;
	std::string newstr;
	int i = 0;
	int center_x = 0;
	while(std::getline(ss, tmp, '\n'))
	{
        center_x = (window.X - tmp.size()) / 2; 
		newstr += std::string(center_x, ' ') + tmp + '\n';
		i++;
	}
    const int center_y = window.Y / 2 - i;
	newstr = std::string(center_y, '\n') + newstr + '\n';
	return newstr;
}
void text::load_string(const std::string& string)
{
    int len_ctr_max = dispatcher->get_windows_api()->get_window_size().X;
    std::stringstream ss(string);
    std::string tmp;
    while (std::getline(ss, tmp, '\n'))
    {
        if (tmp.length() == 0)
        {
            string_data tmp2;
            tmp2.line = "";
            std::vector<std::string> blank{""};
            tmp2.displayable_substring = blank;
            string_objs.push_back(tmp2);
            continue;
        }
        string_data tmp2;
        tmp2.line = tmp;
        for (int i = 0; i < tmp2.line.length(); i += len_ctr_max - 1)
        {
            tmp2.displayable_substring.push_back(tmp2.line.substr(i, len_ctr_max - 1));
            if (i > 0)
            {
                tmp2.displayable_substring[0] += "$";
            }
        }
        *(--tmp2.displayable_substring.end()) += "\n";
        string_objs.push_back(tmp2);
    }
}

void text::do_wrap(string_data& str, int mov) {}

void text::blit_to_screen_from_internal_buffer()
{
    int start_x = 0;
    int start_y = 0;
    int screen_size_x = dispatcher->get_windows_api()->get_window_size().X;
    int end_x = screen_size_x;
    int end_y = 0;
    std::string full;
    for (const auto& sobj : string_objs)
    {
        end_y = 1 + start_y;
        full.append(sobj.displayable_substring[0]);
        start_y = end_y;
    }
    text::print_to_console(full, start_x, 0, end_x, end_y);
}

void text::print_to_console(const std::string& basic_string, int start_x, int start_y, int end_x, int end_y) const
{
    int x_size = dispatcher->get_windows_api()->get_window_size().X;
    int y_size = dispatcher->get_windows_api()->get_window_size().Y;
    SMALL_RECT output_rect = {
        static_cast<short>(start_x),
        static_cast<short>(start_y),
        static_cast<short>(x_size),
        static_cast<short>(y_size)
    };
    COORD buffersiz = {static_cast<short>(x_size),static_cast<short>(end_y)};
    COORD start_coord = {static_cast<short>(start_x),static_cast<short>(start_y)};

    int array_pos = 0;
    auto print_buf = new CHAR_INFO[basic_string.length()];
    for (const auto& sb : basic_string)
    {
        print_buf[array_pos].Char.AsciiChar = sb;
        print_buf[array_pos].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY | FOREGROUND_RED;
        array_pos++;
    }
    WriteConsoleOutputA(
                        output_handle,
                        print_buf,
                        buffersiz,
                        start_coord,
                        &output_rect
                       );
    free(print_buf);

}
