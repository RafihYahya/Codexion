#include "main.h"

void print_lock(struct s_globalstate *gstate, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    pthread_mutex_lock(&gstate->print_lock);
    vprintf(fmt, args);
    pthread_mutex_unlock(&gstate->print_lock);
    va_end(args);
}