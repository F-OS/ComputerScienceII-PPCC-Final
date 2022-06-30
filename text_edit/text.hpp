#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H
#include "dispatch.hpp"


struct string_data
{
    std::string line;
    std::string displayable_substring;
    bool does_wrap = false;
};


class text
{
public:
    explicit text(dispatch& dispatcher);

    COORD get_cursor() const;
	std::string center_text(const std::string& str);

    std::vector<string_data> vectorize_string(std::string buf);

private:
	dispatch* dispatcher;
};

#endif
