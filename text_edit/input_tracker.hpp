#ifndef INPUT_TRACKER_H
#define INPUT_TRACKER_H
#include <memory>
#include <string>
#include <thread>

#include "dispatch.hpp"

class input_tracker
{
public:
	input_tracker();
	~input_tracker();
	input_tracker(dispatch& dispatch_pass);
private:
	void Iinput_tracker_thread() const;
	dispatch* dispatcher;
    std::thread tracker_thread;
	bool killflag = false;
};

#endif
