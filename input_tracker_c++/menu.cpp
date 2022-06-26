


#include "menu.h"

menu::menu(dispatch& dispatch) : dispatcher(&dispatch)
{
}

main_menu::main_menu(dispatch& dispatch_pass) : menu(dispatch_pass)
{
	name = "Main Menu";
	prefix = "~";
	cursor = "->";
	menufunctions.emplace_back("Help", std::bind(&main_menu::help, this));
	menufunctions.emplace_back("New File", std::bind(&main_menu::new_file, this));
	menufunctions.emplace_back("Open File", std::bind(&main_menu::open_file, this));
	menufunctions.emplace_back("Quit", std::bind(&main_menu::exit, this));
	for (const auto& [first, second] : menufunctions)
	{
		menu_items.push_back(first);
	}
}

void main_menu::help()
{
	std::cout << "I'm helping!" << std::endl;
}

void main_menu::new_file()
{
	std::cout << "Out with the old, in with the new" << std::endl;
}

void main_menu::open_file()
{
	std::cout << "Open sesame" << std::endl;
}

void main_menu::exit()
{
	std::cout << "Goodbye!" << std::endl;
	dispatcher->exit_program();
}

void main_menu::displaymenu()
{
	const int response = menu_renderer("OPTIONS", "[*]", "->", menu_items);
	menufunctions[response - 1].second();
}


int menu::menu_renderer(const std::string& menu_name, const std::string& menu_prefix, const std::string& menu_cursor,
                        const std::vector<std::string>& items) const
{
	int tmp = 0;
	int chosen_opt = 1;
	const size_t items_len = items.size();
	short arrow = 0;
	short arrow_old = 0;
	std::string format = "";
	do
	{
		switch (arrow_old)
		{
			// All fallthroughs intended.
		case 1:
		case 3:
			chosen_opt--;
			break;
		case 2:
		case 4:
			chosen_opt++;
		default:
			break;
		}
		chosen_opt = max(1, min(chosen_opt, items_len));
		format = menu_prefix + menu_name + ":";
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
			arrow = dispatcher->s_pop_latest_message_or_return_0(MENU_MSG_CODE);
		}
		arrow_old = arrow;
		arrow = 0;
		system("cls");
	}
	while (arrow_old != 5);
	return chosen_opt;
}
