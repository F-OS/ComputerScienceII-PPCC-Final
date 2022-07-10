#include <iostream>

#include "dispatch.hpp"
#include "file_handler.hpp"
#include "mainhead.h"
#include "menu.hpp"
#include "message_handler.h"
#include "stdio.h"
#include "text.hpp"
#include "windowsapi.hpp"
message_handler global_message_handler;
int main()
{
    dispatch main_dispatch;
    main_menu menu;
    while (true)
    {
        const menu_options menusel = menu.displaymenu();
        switch (menusel)
        {
            case menu_options::HELP:
            {
                const std::string help_text = R"(Welcome to Textedit, a console text editor!
This editor offers nano-style cursor control. You can navigate text using the four arrow keys,
the Page Up and Page Down keys,
the Home and End keys,
and by searching for text with CTRL-G
If you want to save and quit to the main menu, use CTRL-Q
Ctrl-C and Ctrl-V copy and paste, respectively.
If you need to immediately exit without saving, use CTRL-!
To save your work, press CTRL-S, to save your work to a new directory, press CTRL-F
Any issues can be reported at: https://github.com/F-OS/ComputerScienceII-PPCC-Final

You can exit this window by pressing CTRL-Q, too!
)";
                const std::string center = main_dispatch.get_text_obj()->center_text(help_text);
                std::cout << center;
                while (true)
                {
                    if (global_message_handler.get_flag(program_flags::CTRL_Q_PRESSED))
                    {
                        global_message_handler.clear_flag(program_flags::CTRL_Q_PRESSED);
                        system("cls");
                        break;
                    }
                    Sleep(10);
                }
            }
            case menu_options::NEW:
            {
                std::string file_to_open;
                std::cout << "Please enter a file name:" << std::endl;
                std::cin >> file_to_open;
                try
                {
                    main_dispatch.get_file_obj()->open(file_to_open, false);
                }
                catch (std::exception& e)
                {
                    system("cls");
                    std::cout << "Error!" << std::endl << e.what() << std::endl;
                    break;
                }
                const std::string reference_str = main_dispatch.get_file_obj()->read();
                main_dispatch.get_text_obj()->load_string(reference_str);
                main_dispatch.get_cursor_obj()->start_thread();
                main_dispatch.get_windows_api()->set_cursor(0, 0);
                while (!global_message_handler.get_flag(program_flags::CTRL_Q_PRESSED))
                {
                    main_dispatch.get_text_obj()->blit_to_screen_from_internal_buffer();
                    Sleep(1);
                }
                main_dispatch.get_cursor_obj()->stop_thread();
                system("cls");
                break;
            }
            case menu_options::OPEN:
                break;
            case menu_options::QUIT:
                return 0;
        }
    }
}
