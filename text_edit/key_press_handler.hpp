#ifndef KEY_PRESS_HANDLER_H
#define KEY_PRESS_HANDLER_H
#include "dispatch.hpp"
#include "Keytable.hpp"
#include "mainhead.h"
#include "Windows.h"
class dispatch;


class key_press_handler
{
public:
    dispatch* get_dispatcher();

    key_press_handler(dispatch& disp);

    virtual bool process_keypress(const KEY_EVENT_RECORD& keypress) = 0;

protected:
    ~key_press_handler() = default;

private:
    virtual void call_functions() = 0;

    virtual void call_functions(const KEY_EVENT_RECORD& keypress) = 0;

    dispatch* dispatcher;
};


class ctrl_q final : public key_press_handler
{
public:
    explicit ctrl_q(dispatch& disp) : key_press_handler(disp) { }

    bool process_keypress(const KEY_EVENT_RECORD& keypress) override;

private:
    void call_functions() override;

    void call_functions(const KEY_EVENT_RECORD& keypress) override {}
};


class ctrl_c final : public key_press_handler
{
public:
    explicit ctrl_c(dispatch& disp) : key_press_handler(disp) { }

    bool process_keypress(const KEY_EVENT_RECORD& keypress) override;
private:
    void call_functions() override {}

    void call_functions(const KEY_EVENT_RECORD& keypress) override;
};


class forward_to_buffer final : public key_press_handler
{
public:
    bool process_keypress(const KEY_EVENT_RECORD& keypress) override;

    explicit forward_to_buffer(dispatch& disp) : key_press_handler(disp) { }

private:
    void call_functions(const KEY_EVENT_RECORD& keypress) override;

    void call_functions() override {}
};


class cursor_movement_related_keys final : public key_press_handler
{
public:
    bool process_keypress(const KEY_EVENT_RECORD& keypress) override;

    explicit cursor_movement_related_keys(dispatch& disp) : key_press_handler(disp) { }

private:
    void call_functions() override { }

    void call_functions(const KEY_EVENT_RECORD& keypress);
};

#endif
