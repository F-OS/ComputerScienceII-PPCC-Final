#include "cursor.h"

#include "text.hpp"
#include "windowsapi.hpp"
cursor::cursor(dispatch& dispatch)
{
    dispatcher = &dispatch;
    cursorloc = {0,0};
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
    while (!(dispatcher->is_shutting_down() || quitflag))
    {
        cursorloc = dispatcher->get_windows_api()->get_cursor();
        const int arrow = dispatcher->s_pop_latest_message_or_return_0(message_tags::CURSOR_MOVEMENT_LISTENER);
        int newcursor_x = cursorloc.X;
        int newcursor_y = cursorloc.Y;

        if (arrow == 0 || dispatcher->get_lock_on_key_state())
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
        }
        if (newcursor_x > window_x)
        {
            newcursor_x = 0;
            newcursor_y += 1;
        }
        if (newcursor_x < 0)
        {
            newcursor_x = dispatcher->get_text_obj()->get_length_at(newcursor_y - 1);
            newcursor_y -= 1;
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
        dispatcher->get_windows_api()->set_cursor(newcursor_x, newcursor_y);
        Sleep(15);
    }
}
