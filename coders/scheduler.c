#include "main.h"
struct s_SchedulerState {

};


int init_scheduler(char *str, struct s_Queue *q, struct s_ArgvParsedConfig config)
{
    int err;

    err = 0;
    struct s_SchedulerState state;

    if (strcmp(str, "fifo"))
    {
        err = fifo_scheduler(state);
    }
    else if (strcmp(str, "edf"))
    {
        err = edf_scheduler(state);
    }
    else 
    {
        ERROR("Input Invalid");
        return (-1);
    }
    return (err);
}


int fifo_scheduler()
{

}


int edf_scheduler()
{

}