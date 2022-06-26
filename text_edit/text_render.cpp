#include "text_render.hpp"

text_render::text_render(dispatch& dispatcher)
{
	this->dispatcher = &dispatcher;
}

void text_render::center_text(const std::string& string)
{
	auto output = dispatcher->request_io_handle(0);
	auto screen_width = dispatcher->get_window_size();
}

COORD text_render::get_screen_center(std::string center)
{
	auto screen = dispatcher->get_window_size();
	screen.X = (screen.X - center.length()) / 2;
	screen.Y = (screen.Y) / 2;
	return screen;
}
