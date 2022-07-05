#ifndef MENU_H
#define MENU_H
#include <functional>

#include "dispatch.hpp"
#include "mainhead.h"


class main_menu
{
public:
    // Menu elements shouldn't be moved. Only one should exist as the menu_renderer function dominates the screen.
    main_menu(const main_menu& other) = delete;

    main_menu(main_menu&& other) = delete;

    main_menu& operator=(const main_menu& other) = delete;

    main_menu& operator=(main_menu&& other) = delete;

    explicit main_menu(dispatch& dispatch);

    void help();

    void new_file();

    void text_buf_test();

    void open_file();

    void exit();

    void displaymenu();

private:
    dispatch* dispatcher;

    int menu_renderer(
        const std::string& menu_name,
        const std::string& menu_prefix,
        const std::string& menu_cursor,
        const std::vector<std::string>& items
        ) const;

    std::string name;
    std::string prefix;
    std::string cursor;
    std::vector<std::pair<std::string, std::function<void()>>> menufunctions;
    std::vector<std::string> menu_items;
    bool exitflag = true;
};
#endif
