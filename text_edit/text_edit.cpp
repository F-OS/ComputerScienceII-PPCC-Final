#include "dispatch.hpp"
#include "file_handler.hpp"
#include "input_tracker.hpp"
#include "menu.hpp"
#include "text.hpp"
#include "text_buffer.hpp"

#define SECOND * 1000

int main()
{
	dispatch main_dispatch;
	while (true)
	{
		main_dispatch.spawn_menu();
	}

}
