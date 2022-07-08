#ifndef INPUT_TRACKER_H
#define INPUT_TRACKER_H
#include <thread>
#include "Windows.h"
#include "dispatch.hpp"
#include "mainhead.h"


class input_tracker
{
public:
    ~input_tracker();

    input_tracker(dispatch& dispatch_pass);

private:
    void input_tracker_thread() const;

    dispatch* dispatcher;
    std::thread tracker_thread;
};

#endif
