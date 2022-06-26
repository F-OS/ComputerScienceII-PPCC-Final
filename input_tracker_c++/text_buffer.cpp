// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "text_buffer.h"

text_buffer::text_buffer(dispatch& dispatch)
{
	dispatcher = &dispatch;
}
