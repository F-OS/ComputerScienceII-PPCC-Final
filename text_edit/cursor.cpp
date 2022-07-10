#include "cursor.h"

#include "Keytable.hpp"
#include "message_handler.h"
#include "text.hpp"
#include "windowsapi.hpp"

cursor::cursor(dispatch& dispatch)
{
    dispatcher = &dispatch;
    window_x = dispatcher->get_windows_api()->get_window_size().X;
    window_y = dispatcher->get_windows_api()->get_window_size().Y;
}
void cursor::start_thread()
{
    tracker_thread = std::thread(&cursor::cursor_tracking_thread, this);
}

void cursor::stop_thread()
{
    quitflag = true;
    tracker_thread.join();
}
void cursor::cursor_tracking_thread()
{
    const COORD cursorloc = dispatcher->get_windows_api()->get_cursor();
    int newcursor_x = cursorloc.X;
    int newcursor_y = cursorloc.Y;
    while (!(dispatcher->is_shutting_down() || quitflag))
    {
        const int arrow = global_message_handler.s_pop_latest_message_or_return_0(
                                                                                  message_tags::CURSOR_MOVEMENT_LISTENER
                                                                                 );
        if (arrow == 0)
        {
            continue;
        }
        switch (arrow)
        {
            case LEFT_ARROW:
                newcursor_x -= 1;
                break;
            case RIGHT_ARROW:
                newcursor_x += 1;
                break;
            case UP_ARROW:
                newcursor_y -= 1;
                break;
            case DOWN_ARROW:
                newcursor_y += 1;
                break;
            case RETURN_PRESS:
                global_message_handler.c_new_message('\n', message_tags::BUFFER_MSG_CODE);
                newcursor_y += 1;
                newcursor_x = 0;
                break;
        }
        if (newcursor_x > window_x)
        {
            newcursor_x = 0;
            newcursor_y += 1;
        }
        if (newcursor_x < 0)
        {
            newcursor_y -= 1;
            newcursor_x = dispatcher->get_text_obj()->get_length_at(newcursor_y);
        }
        if (newcursor_y >= window_y - 1)
        {
            dispatcher->get_text_obj()->do_scroll(1);
            newcursor_y = cursorloc.Y;
        }
        if (newcursor_y < 0)
        {
            dispatcher->get_text_obj()->do_scroll(-1);
            newcursor_y = cursorloc.Y;
        }
        if (newcursor_x > dispatcher->get_text_obj()->get_length_at(newcursor_y))
        {
            newcursor_x = 0;
            newcursor_y += 1;
        }
        cursor_x = newcursor_x;
        cursor_y = newcursor_y;
        global_message_handler.set_flag(program_flags::CURSOR_ALTERED);
        global_message_handler.clear_flag(program_flags::KEYBOARD_OP_LOCK);
        Sleep(15);
    }
}

COORD cursor::get_cursor()
{
    return COORD{static_cast<short>(cursor_x),static_cast<short>(cursor_y)};
}
