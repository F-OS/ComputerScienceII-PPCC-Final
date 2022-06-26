#ifndef KEY_PRESS_HANDLER_H
#define KEY_PRESS_HANDLER_H
#include <Windows.h>
#include <iostream>
#include "dispatch.hpp"
#include "Keytable.hpp"
class dispatch;

class key_press_handler
{
public:
	dispatch* get_dispatcher();
	key_press_handler(dispatch& disp, bool ctrl, bool alt, int key);
	virtual bool process_keypress(KEY_EVENT_RECORD keypress);
private:
	bool req_ctrl_key = false;
	bool req_alt_key = false;
	int req_keypress = 0;
	virtual void call_functions() = 0;
	dispatch* dispatcher;
protected:
	~key_press_handler() = default;
};

class ctrl_q final : public key_press_handler
{
public:
	explicit ctrl_q(dispatch& disp) : key_press_handler(disp, true, false, KEY_PRESS_Q) {}

private:
	void call_functions() override;
};

class ctrl_c final : public key_press_handler
{
public:
	explicit ctrl_c(dispatch& disp) : key_press_handler(disp, true, false, KEY_PRESS_C) {}

private:
	void call_functions() override {/*TODO: Copy function */}
};

class forward_to_buffer final : public key_press_handler
{
public:
	bool process_keypress(KEY_EVENT_RECORD keypress) override;
	explicit forward_to_buffer(dispatch& disp) : key_press_handler(disp, false, false, 0) {}

private:
	void call_functions() override {/* Overridden to prevent compiler complaints and unneeded calls. */ }
	void call_functions(const KEY_EVENT_RECORD& keypress);
};

class menu_hooked_keys final : public key_press_handler
{
public:
	bool process_keypress(KEY_EVENT_RECORD keypress) override;
	explicit menu_hooked_keys(dispatch& disp) : key_press_handler(disp, true, false, KEY_PRESS_C) {}

private:
	void call_functions() override {}
	void call_functions(const KEY_EVENT_RECORD& keypress);
};


#endif
