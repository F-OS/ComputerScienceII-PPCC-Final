#pragma once
#include <thread>
#include "mainhead.h"
#include "Windows.h"


class cursor
{
public:
    explicit cursor(dispatch& dispatch);

    void start_thread();

    void stop_thread();

    COORD get_cursor();

private:
    void cursor_tracking_thread();



    dispatch* dispatcher;
    std::thread tracker_thread;
    int cursor_x = 0;
    int cursor_y = 0;
    int window_x = 0;
    int window_y = 0;
    bool quitflag = false;
};
