#include "main.h"



int init_scheduler(struct s_globalstate *gstate)
{
    int     err;

    err = 0;
    if (strcmp(gstate->pconfig->scheduler, "fifo"))
        err = fifo_scheduler(gstate);
    else if (strcmp(gstate->pconfig->scheduler, "edf"))
        err = edf_scheduler();
    else 
    {
        ERROR("Input Invalid");
        return (-1);
    }
    return (err);
}
