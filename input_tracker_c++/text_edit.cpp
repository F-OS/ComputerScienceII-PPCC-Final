// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "dispatch.h"
#include "file_handler.h"
#include "input_tracker.h"
#include "menu.h"
#include "text_buffer.h"
#include "text_render.h"

#define SECOND * 1000

int main()
{
	dispatch main_dispatch;
	std::string path = R"(C:\Users\Reagan Fischer\source\repos\text_editor\text_editor\gutenberg)";
	text_render main_render(main_dispatch);
	text_buffer main_buffer(main_dispatch);
	input_tracker input(main_dispatch);
	main_menu menu(main_dispatch);
	file_handler file_handle(path, main_dispatch);
	std::string newstr = file_handle.read();
	file_handle.write(newstr);
	while (true)
	{
		menu.displaymenu();
		std::cout << "I'm still here!" << std::endl;
		Sleep(0.2 SECOND);
		if (main_dispatch.is_shutting_down())
		{
			break;
		}
	}
}
