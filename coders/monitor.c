#include "thread.h"

void *monitor_thread(void *arg)
{

}

int init_monitor_thread(struct s_globalstate *gstate, pthread_t **monitor)
{

    gstate->mstate = malloc(sizeof(struct s_monitorstate));
    if (!gstate->mstate)
        return (-1);
    if (pthread_mutex_init(&(gstate->mstate->death_lock), NULL) < 0)
        return (free(gstate->mstate), -1);
    if (!monitor)
    {
        ERROR("Can't be Null");
        return (free(gstate->mstate), -1);
    }
    if (pthread_create(*monitor, NULL, NULL, NULL) < 0 )
    {
        ERROR("Failed Creation of the Thread");
        return (free(gstate->mstate), -1);
    }
    return (0);
}