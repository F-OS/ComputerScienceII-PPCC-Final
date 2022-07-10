#ifndef MENU_H
#define MENU_H
#include <string>
#include <vector>


enum class menu_options { HELP, NEW, OPEN, QUIT };


class main_menu
{
public:
    // Menu elements shouldn't be moved. Only one should exist as the menu_renderer function dominates the screen.
    main_menu(const main_menu& other) = delete;

    main_menu(main_menu&& other) = delete;

    main_menu& operator=(const main_menu& other) = delete;

    main_menu& operator=(main_menu&& other) = delete;

    explicit main_menu();

    menu_options displaymenu();

private:
    int menu_renderer(
        const std::string& menu_name,
        const std::string& menu_prefix,
        const std::string& menu_cursor,
        const std::vector<std::string>& items
        ) const;

    std::string name;
    std::string prefix;
    std::string cursor;
    std::vector<std::string> menu_items;
};
#endif
