#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H
#include <memory>
#include <string>
#include <stdexcept>

#include "dispatch.hpp"
#include "text_buffer.hpp"

class file_handler
{
public:
	file_handler(std::string path, dispatch& dispatch_pass);
	std::string read() const;
	void write(const std::string& a) const;
	bool ready_for_exit();

private:
	std::string file_path;
	std::unique_ptr<FILE, decltype(&fclose)> current_handle{ nullptr, &fclose };
	dispatch* dispatcher;
};

#endif
