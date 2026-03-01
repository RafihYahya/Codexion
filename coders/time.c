#include "main.h"

// time helper funcs

size_t get_curr_time_ms()
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
    {
        ERROR("Can't get the time");
        return (-1);
    }
    size_t ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return ms;
}