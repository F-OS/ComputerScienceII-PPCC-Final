#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H
#include <fstream>
#include <string>


class file_handler
{
public:
    file_handler(std::string path);

    void validate_file_path(std::string& path, bool must_exist);

    void open(std::string& path, bool must_exist);

    std::string read();

    void write(const std::string& a);

    bool ready_for_exit();

private:
    std::string file_path;
    std::fstream fs;
    bool handle_open = false;
};

#endif
