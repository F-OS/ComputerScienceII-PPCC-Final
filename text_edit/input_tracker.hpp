#ifndef INPUT_TRACKER_H
#define INPUT_TRACKER_H
#include <thread>
#include <vector>

#include "mainhead.h"
#include "Windows.h"


class input_tracker
{
public:
    const std::vector<KEY_EVENT_RECORD>& get_keybuffer() const;

    const std::vector<WINDOW_BUFFER_SIZE_RECORD>& get_windowbuffer() const;

    bool get_thread_lock() const;

    void set_thread_lock();

    void clear_thread_lock();

    ~input_tracker();

    input_tracker();

    void kill_thread();

private:
    void input_tracker_thread();

    void send_to_key_handler();

    std::vector<std::shared_ptr<key_press_handler>> hooked_keypresses;
    std::vector<KEY_EVENT_RECORD> keybuffer;
    std::vector<WINDOW_BUFFER_SIZE_RECORD> windowbuffer;
    std::thread tracker_thread;
    bool active = false;
    bool endthread = false;
    bool keyslocked = false;
};

#endif
