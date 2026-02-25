#include "main.h"

int fifo_scheduler(struct s_globalstate *gstate)
{
    if (!gstate)
        return (-1);
    gstate->scheduler = malloc(sizeof(struct s_scheduler));
    if (!(gstate->scheduler))
    {
        ERROR("Malloc Fifo Scheduler Failed");
        return (-1);
    }
    if (pthread_mutex_init(&(gstate->scheduler->sched_lock), NULL) < 0)
    {
        ERROR("Init schefuler Fifo Mutex Failed");
        return (-1);
    }
}