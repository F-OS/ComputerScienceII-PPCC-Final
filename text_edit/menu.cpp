#include "menu.hpp"
#include "text.hpp"
#include "windowsapi.hpp"
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
    menufunctions.emplace_back("Test cool text wrap", std::bind(&main_menu::text_buf_test, this));
	for (const auto& [first, second] : menufunctions)
	{
		menu_items.push_back(first);
	}
}

void main_menu::help()
{
	std::string help_text = R"(Welcome to Textedit, a console text editor!
This editor offers nano-style cursor control. You can navigate text using the four arrow keys,
the Page Up and Page Down keys,
the Home and End keys,
and by searching for text with CTRL-G
If you want to save and quit to the main menu, use CTRL-Q
Ctrl-C and Ctrl-V copy and paste, respectively.
If you need to immediately exit without saving, use CTRL-!
To save your work, press CTRL-S, to save your work to a new directory, press CTRL-F
Any issues can be reported at: https://github.com/F-OS/ComputerScienceII-PPCC-Final
	)";
    std::string center = dispatcher->get_text_obj()->center_text(help_text);
	std::cout << center;
	short arrow = 0;
	while (arrow != 5)
	{
		Sleep(10);
		arrow = dispatcher->s_pop_latest_message_or_return_0(MENU_MSG_CODE);
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
    std::cout << "Text buffer test" << std::endl;
    std::string reference_str =
        R"(
It is a long established fact that a reader will be distracted by the readable content of a page when looking at its layout.
The point of using Lorem Ipsum is that it has a more-or-less normal distribution of letters, as opposed to using 'Content here, content here', 
making it look like readable English. Many desktop publishing packages and web page editors now use Lorem Ipsum as their default model text,
and a search for 'lorem ipsum' will uncover many web sites still in their infancy. Various versions have evolved over the years,
sometimes by accident, sometimes on purpose (injected humour and the like).
)";
    dispatcher->get_text_obj()->load_string(reference_str);
    dispatcher->get_text_obj()->blit_to_screen_from_internal_buffer();
}

void main_menu::open_file()
{
	std::cout << "Open sesame" << std::endl;
    //	exitflag = false;
}

void main_menu::exit()
{
	std::cout << "Goodbye!" << std::endl;
	dispatcher->exit_program();
}

void main_menu::displaymenu()
{
    int response;
    while (exitflag)
    {
        response = menu_renderer("OPTIONS", "[*]", "->", menu_items);
        menufunctions[response - 1].second();
    }
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
	} while (arrow_old != 5);
	return chosen_opt;
}
