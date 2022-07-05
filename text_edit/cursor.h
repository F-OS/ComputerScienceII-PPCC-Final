#pragma once
#include <thread>

#include "dispatch.hpp"


class cursor
{
public:
    explicit cursor(dispatch& dispatch);

    void start_thread();

    void stop_thread();

private:
    void cursor_tracking_thread();

    dispatch* dispatcher;
    std::thread tracker_thread;
    COORD cursorloc;
    int window_x;
    int window_y;
    bool quitflag = false;
};
