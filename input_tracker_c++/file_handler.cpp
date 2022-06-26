


#include "file_handler.h"


file_handler::file_handler(std::string path, dispatch& dispatch_pass) : file_path(std::move(path)), dispatcher(&dispatch_pass)
{
	FILE* f = nullptr;
	errno_t err = fopen_s(&f, file_path.c_str(), "r");
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
		throw std::runtime_error(std::string("File handler unable to create file, error code: " + std::to_string(err)));
	}
	current_handle.reset(f);
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
