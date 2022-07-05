#include "menu.hpp"

#include <iostream>

#include "cursor.h"
#include "file_handler.hpp"
#include "text.hpp"
#include "windowsapi.hpp"
main_menu::main_menu(dispatch& dispatch_pass) : dispatcher(&dispatch_pass)
{
    dispatcher->get_file_obj()->open("C:\\Tools\\shakesphere.txt");
    const std::string reference_str = dispatcher->get_file_obj()->read();
    dispatcher->get_text_obj()->load_string(reference_str);
    name = "Main Menu";
    prefix = "~";
    cursor = "->";
    menufunctions.emplace_back("Help", std::bind(&main_menu::help, this));
    menufunctions.emplace_back("New File", std::bind(&main_menu::new_file, this));
    menufunctions.emplace_back("Open File", std::bind(&main_menu::open_file, this));
    menufunctions.emplace_back("Quit", std::bind(&main_menu::quit_menu, this));
    menufunctions.emplace_back("Test cool text wrap", std::bind(&main_menu::text_buf_test, this));
    for (const auto& [first, second] : menufunctions)
    {
        menu_items.push_back(first);
    }
}

void main_menu::help()
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
    const std::string center = dispatcher->get_text_obj()->center_text(help_text);
    std::cout << center;
    while (!dispatcher->is_return_to_main())
    {
        Sleep(10);
    }
    system("cls");
}

void main_menu::new_file()
{
    std::cout << "Out with the old, in with the new" << std::endl;
    // exitflag = false;
}

void main_menu::text_buf_test()
{
    dispatcher->get_text_obj()->blit_to_screen_from_internal_buffer();
    dispatcher->get_cursor_obj()->start_thread();
    dispatcher->get_windows_api()->set_cursor(0, 0);
    while (!dispatcher->is_return_to_main())
    {
        Sleep(10);
    }
    dispatcher->get_cursor_obj()->stop_thread();
    system("cls");
}

void main_menu::open_file()
{
    std::cout << "Open sesame" << std::endl;
    //	exitflag = false;
}

void main_menu::quit_menu()
{
    std::cout << "Goodbye!" << std::endl;
    dispatcher->exit_program();
}

void main_menu::displaymenu()
{
    int response = menu_renderer("OPTIONS", "[*]", "->", menu_items);
    menufunctions[response - 1].second();
}

int main_menu::menu_renderer(
    const std::string& menu_name,
    const std::string& menu_prefix,
    const std::string& menu_cursor,
    const std::vector<std::string>& items
    ) const
{
    int tmp = 0;
    int chosen_opt = 1;
    const size_t items_len = items.size();
    short arrow = 0;
    short arrow_old = 0;
    do
    {
        switch (arrow_old)
        {
            // All fallthroughs intended.
            case 1:
                chosen_opt--;
                break;
            case 3:
                chosen_opt--;
                break;
            case 2:
                chosen_opt++;
                break;
            case 4:
                chosen_opt++;
                break;
            default:
                break;
        }
        chosen_opt = max(1, min(chosen_opt, items_len));
        std::string format = menu_prefix + menu_name + ":";
        for (const auto& item : items)
        {
            format += "\n";
            int pos = 5;
            while (pos-- > 0)
            {
                format += " ";
            }
            if (tmp + 1 == chosen_opt)
            {
                format += "\b" + menu_cursor;
            }
            format += item;
            tmp++;
        }
        tmp = 0;
        std::cout << format;
        while (arrow == 0)
        {
            Sleep(10);
            arrow = dispatcher->s_pop_latest_message_or_return_0(message_tags::CURSOR_MOVEMENT_LISTENER);
        }
        arrow_old = arrow;
        arrow = 0;
        system("cls");
    }
    while (arrow_old != 5);
    return chosen_opt;
}
