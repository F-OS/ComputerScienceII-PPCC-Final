#ifndef MAIN_HEAD_H
#define MAIN_HEAD_H
// Forward Declares
#include "Windows.h"
class windowsapi;
class dispatch;
class text;
class key_press_handler;
class file_handler;
class input_tracker;
class menu;
//Options
constexpr SMALL_RECT C_window_size{0,0,121,30};
constexpr int C_window_type = GWL_STYLE;
constexpr COORD C_buffer_size{121,30};
constexpr auto C_apptitle = "TextEdit";
#endif
