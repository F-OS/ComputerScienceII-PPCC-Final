#include "text_render.hpp"

#include <sstream>

text_render::text_render(dispatch& dispatcher)
{
	this->dispatcher = &dispatcher;
	screen_size = this->dispatcher->get_window_size();
	output = this->dispatcher->request_io_handle(0);

}

std::string text_render::center_text(const std::string& str)
{
	COORD window = dispatcher->get_window_size();
	std::stringstream ss(str);
	std::string tmp;
	std::string newstr;
	int i = 0;
	int center_x = 0;
	while(std::getline(ss, tmp, '\n'))
	{
		center_x = ((window.X/2) - tmp.size()) / 2; 
		newstr += std::string(center_x, ' ') + tmp + '\n';
		i++;
	}
	int center_y = (window.Y / 2) - (i*2);
	newstr = std::string(center_y, '\n') + newstr + '\n';
	return newstr;
}

void text_render::write_ascii_to_console(const char* buf, size_t size, const COORD& coord)
{
	HANDLE console = dispatcher->open_screen_buf();
	dispatcher->set_cursor(console, coord.X, coord.Y);
	WriteConsoleA(console, buf, size, NULL, NULL);
}

COORD text_render::get_cursor()
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi = dispatcher->get_cbsi();
	return cbsi.dwCursorPosition;

}