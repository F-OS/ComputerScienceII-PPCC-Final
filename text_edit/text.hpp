#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H
#include <string>
#include <vector>
#include "dispatch.hpp"
#include "mainhead.h"


// Non-Class objects
struct string_data
{
    // This is our string from newline to newline.
    std::string line;
    // This is a vector of strings for display
    std::vector<std::string> displayable_substring;
};


class text
{
public:
    explicit text(dispatch& dispatch_pass);

    COORD get_cursor() const;

    std::string center_text(const std::string& str);

    void do_wrap(string_data& str, int mov);

    void load_attributes() const;

    void display_whole_buffer();

    void print_to_whole_console(const std::string& basic_string);

    void blit_to_screen_from_internal_buffer();

    void load_string(const std::string& string);

    SHORT get_length_at(int y_loc);

    void do_scroll(int i);

private:
    dispatch* dispatcher;
    std::vector<string_data> string_objs;
    HANDLE output_handle;
    int x_cursor_pos = 0;
    int y_cursor_pos = 0;
    CHAR_INFO* print_buf;
    COORD window;
    int currentoffset = 0;
};

#endif
