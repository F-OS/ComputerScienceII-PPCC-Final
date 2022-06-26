#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H
#include "dispatch.hpp"

class text_render
{
public:
	explicit text_render(dispatch& dispatcher);
	void center_text(const std::string& string);
	COORD get_screen_center(std::string center);
private:
	dispatch* dispatcher;
	COORD get_window_size();
};

#endif
