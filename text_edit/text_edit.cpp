#include "dispatch.hpp"
#include "file_handler.hpp"
#include "input_tracker.hpp"
#include "menu.hpp"
#include "text_buffer.hpp"
#include "text_render.hpp"

#define SECOND * 1000

int main()
{
	dispatch main_dispatch;
	std::string path = R"(C:\Users\Reagan Fischer\source\repos\text_editor\text_editor\gutenberg)";
	text_buffer main_buffer(main_dispatch);
	input_tracker input(main_dispatch);
	main_menu menu(main_dispatch);
	file_handler file_handle(path, main_dispatch);
	std::string newstr = file_handle.read();
	file_handle.write(newstr);
	while (true)
	{
		menu.displaymenu();
		if (main_dispatch.is_shutting_down())
		{
			if(main_buffer.ready_for_exit() && file_handle.ready_for_exit())
			{
				exit(EXIT_SUCCESS);
			}
		}
	}

}
