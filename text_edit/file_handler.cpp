#include "file_handler.hpp"
#include <regex>
#include <sstream>
#include "Windows.h"
file_handler::file_handler(std::string path) : file_path(std::move(path))
{
    if (file_path.length() != 0u)
    {
        open(file_path, false);
    }
}

void file_handler::validate_file_path(std::string& path, bool must_exist)
{
    int i;
    while (std::string::npos != (i = path.find('/')))
    {
        path.replace(i, 1, "\\", 1);
    }
    const std::regex check(R"(([a-zA-Z]\:|\\|.)(\\{1}|((\\{1})[^\\]([^/:*?<>""|]*))+)$)");
    if (!std::regex_match(path, check))
    {
        path = ".\\" + path;
        if (!std::regex_match(path, check))
        {
            throw std::runtime_error(
                                     "Invalid file name: " + path +
                                     R"( fails regex. All file names must start with either the drive letter or ., must use / or \ to mark directories, and cannot contain ^/:*?<>""|)"
                                    );
        }
    }
    if (path[0] == '.')
    {
        char current_directory[MAX_PATH];
        if (GetCurrentDirectoryA(MAX_PATH, current_directory) == 0)
        {
            throw std::runtime_error(
                                     "Get Current Directory returned error code: " + std::to_string(
                                                                                                    static_cast<int>(
                                                                                                        GetLastError())
                                                                                                   )
                                    );
        }
        const std::string prepend(current_directory);
        path.erase(std::remove(path.begin(), path.begin() + 1, '.'));
        path = prepend + path;
    }
    if (must_exist)
    {
        WIN32_FIND_DATAA fd;
        const HANDLE a = FindFirstFileA(path.c_str(), &fd);
        if (a == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error("You have entered a path to a file that does not exist. Did you want New File?");
        }
    }
}

void file_handler::open(std::string& path, bool must_exist)
{
    if (handle_open)
    {
        handle_open = false;
        fs.close();
    }
    validate_file_path(path, must_exist);
    fs.open(path.c_str(), std::fstream::in | std::fstream::out);
    file_path = std::move(path);
    if (!fs.is_open())
    {
        throw std::runtime_error("Attempt to open file failed.");
    }
    handle_open = true;
}

std::string file_handler::read()
{
    std::string buf;
    std::string buffer;
    while (std::getline(fs, buf, '\n'))
    {
        buffer += buf + "\n";
    }
    fs.clear();
    fs.seekg(0);
    return buffer;
}

void file_handler::write(const std::string& a)
{
    fs << a;
    fs.clear();
    fs.seekg(0);
}

bool file_handler::ready_for_exit()
{
    return true; // Todo: define save logic
}
