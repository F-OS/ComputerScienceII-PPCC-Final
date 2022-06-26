#ifndef MENU_H
#define MENU_H
#include <functional>
#include <string>
#include <conio.h>
#include <iostream>
#include "dispatch.hpp"

class menu
{
public:
	// Menu elements shouldn't be moved. Only one should exist as the menu_renderer function dominates the screen.
	menu(const menu& other) = delete;
	menu(menu&& other) = delete;
	menu& operator=(const menu& other) = delete;
	menu& operator=(menu&& other) = delete;
	// Declare a virtual constructor for standards.
	virtual ~menu() = default;
	void passinput(short arrow);
	virtual void displaymenu() = 0;
protected:
	dispatch* dispatcher;
	explicit menu(dispatch& dispatch);
	int menu_renderer(const std::string& menu_name, const std::string& menu_prefix, const std::string& menu_cursor,
		const std::vector<std::string>& items) const;
	std::string name;
	std::string prefix;
	std::string cursor;
	std::vector<std::pair<std::string, std::function<void()>>> menufunctions;
	std::vector<std::string> menu_items;
};

class main_menu : public menu
{
public:
	void help();
	void new_file();
	void open_file();
	void exit();
	void displaymenu() override;
	explicit main_menu(dispatch& dispatch);
};
#endif
