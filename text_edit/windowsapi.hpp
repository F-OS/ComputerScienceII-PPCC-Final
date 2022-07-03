#ifndef WINDOWSAPI_HPP
#define WINDOWSAPI_HPP
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include "Windows.h"


class windowsapi
{
public:
    const HANDLE& request_io_handle(int stream);

    void save_console_mode();

    void load_old_console_mode();

    void set_console_mode(int stream, DWORD bitflags);

    bool console_has_input_buffered();

    INPUT_RECORD* get_console_input_array(unsigned long& buffer_length);

    COORD get_window_size();

    void update_screen_buffer();

    CONSOLE_SCREEN_BUFFER_INFO get_cbsi();

    COORD set_cursor(int x, int y);

    COORD get_cursor();

private:
    HANDLE output_handle = nullptr;
    HANDLE input_handle = nullptr;
    HANDLE error_handle = nullptr;
    CONSOLE_SCREEN_BUFFER_INFO* cbsi = nullptr;
    DWORD old_console_mode_stdio = 0x00000000000;
    DWORD old_console_mode_stdin = 0x00000000000;
    DWORD old_console_mode_stderr = 0x00000000000;
};
#endif