#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H
#include "dispatch.hpp"

class text_render
{
public:
	explicit text_render(dispatch& dispatcher);
	void write_ascii_to_console(const char* buf, size_t size, const COORD& coord);
	COORD get_cursor();
	std::string center_text(const std::string& str);
	COORD center_cursor(std::string center, int i);
private:
	dispatch* dispatcher;
	COORD screen_size;
	HANDLE output;
	COORD coord;
	COORD get_window_size();
};

#endif
