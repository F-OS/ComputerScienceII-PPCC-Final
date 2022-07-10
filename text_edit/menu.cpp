#include "menu.hpp"

#include <iostream>

#include "cursor.h"
#include "file_handler.hpp"
#include "message_handler.h"

main_menu::main_menu()
{
    name = "Main Menu";
    prefix = "~";
    cursor = "->";
    menu_items.push_back("Help");
    menu_items.push_back("New File");
    menu_items.push_back("Open File");
    menu_items.push_back("Quit");
}

menu_options main_menu::displaymenu()
{
    const int response = menu_renderer("OPTIONS", "[*]", "->", menu_items);
    return static_cast<menu_options>(response);
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
            arrow = global_message_handler.s_pop_latest_message_or_return_0(message_tags::CURSOR_MOVEMENT_LISTENER);
        }
        arrow_old = arrow;
        arrow = 0;
        system("cls");
    }
    while (arrow_old != 5);
    return chosen_opt - 1;
}
