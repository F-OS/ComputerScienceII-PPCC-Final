#include "text.hpp"

#include <sstream>

#include "windowsapi.hpp"

text::text(dispatch& dispatcher)
{
	this->dispatcher = &dispatcher;
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

std::vector<string_data> text::vectorize_string(std::string buf)
{
    
}

void text::do_wrap()