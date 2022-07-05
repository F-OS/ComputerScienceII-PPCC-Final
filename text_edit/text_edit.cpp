#include "dispatch.hpp"
#include "mainhead.h"
int main()
{
    dispatch main_dispatch;
    while (true)
    {
        main_dispatch.spawn_menu();
        if (main_dispatch.is_shutting_down())
        {
            break;
        }
    }
}
