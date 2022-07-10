#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H
#include <memory>

#include <fstream>
#include <iostream>
#include <regex>
#include "mainhead.h"
#include "Windows.h"

class file_handler
{
public:
    file_handler(std::string path);

    void validate_file_permissions(std::string& path);

    void validate_file_path(std::string& path, bool must_exist);

    void open(std::string& path, bool must_exist);

    std::string read();

    void write(std::string& a);

    bool ready_for_exit();

private:
    std::string file_path;
    std::fstream fs;
    bool handle_open = false;
};

#endif
