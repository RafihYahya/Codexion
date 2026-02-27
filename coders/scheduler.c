#include "main.h"


int init_scheduler(struct s_globalstate *gstate)
{
    int     r_err;

    r_err = 0;
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
    DEBUG("Finished Scheduler Setup");
    if (strcmp(gstate->pconfig->scheduler, "fifo"))
        r_err = fifo_scheduler(gstate);
    else if (strcmp(gstate->pconfig->scheduler, "edf"))
        r_err = edf_scheduler(gstate);
    else 
    {
        ERROR("Input Invalid");
        return (-1);
    }
    return (r_err);
}
