#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H
#include "dispatch.hpp"


struct string_data
{
    std::string line; // This is our string from newline to newline.
    std::vector<std::string> displayable_substring;
};


class text
{
public:
    explicit text(dispatch& dispatcher);

    COORD get_cursor() const;
	std::string center_text(const std::string& str);

    void do_wrap(string_data& str, int mov);

    void print_to_console(const std::string& basic_string, int start_x, int start_y, int end_x, int end_y) const;

    void blit_to_screen_from_internal_buffer();

    void load_string(const std::string& string);


private:
	dispatch* dispatcher;
    std::vector<string_data> string_objs;
    HANDLE output_handle;
};

#endif
