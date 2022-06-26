#include "text_buffer.hpp"

text_buffer::text_buffer(dispatch& dispatch)
{
	dispatcher = &dispatch;
}

bool text_buffer::ready_for_exit()
{
	return true; //todo: define save logic
}
