#ifndef TEXT_BUFFER_H
#define TEXT_BUFFER_H
#include "dispatch.hpp"

class text_buffer
{
public:
	explicit text_buffer(const dispatch& dispatch);
	text_buffer(dispatch& dispatch);
	dispatch* dispatcher;
	bool ready_for_exit();
};

#endif
