#include "main.h"

void print_lock(struct s_globalstate *gstate, char *msg)
{
    pthread_mutex_lock(&(gstate->print_lock));
    printf("%s", msg);
    pthread_mutex_unlock(&(gstate->print_lock));

    return ;
}