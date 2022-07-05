#include "file_handler.hpp"
#include "stdio.h"
file_handler::file_handler(std::string path) : file_path(std::move(path))
{
    if (file_path.length() == 0u)
    {
        open(file_path);
    }
}

int file_handler::open(const std::string& path)
{
    if ((current_handle.get() != nullptr) && file_path != path)
    {
        fclose(current_handle.get());
        current_handle.reset(nullptr);
    }
    FILE* f = nullptr;
    file_path = path;
    if (file_path.length() == 0u)
    {
        return -1;
    }
    const errno_t err = fopen_s(&f, file_path.c_str(), "r");
    if (err == 0)
    {
        fclose(f);
        fopen_s(&f, file_path.c_str(), "r+");
    }
    else if (err == ENOENT)
    {
        fopen_s(&f, file_path.c_str(), "w");
    }
    else
    {
        return err;
    }
    current_handle.reset(f);
    return 0;
}

std::string file_handler::read() const
{
    std::string buffer;
    char buf[BUFSIZ];
    while (fgets(buf, BUFSIZ, current_handle.get()) != nullptr)
    {
        buffer += std::string(buf);
    }
    return buffer;
}

void file_handler::write(const std::string& a) const
{
    rewind(current_handle.get());
    fputs(a.c_str(), current_handle.get());
}

bool file_handler::ready_for_exit()
{
    return true; // Todo: define save logic
}
