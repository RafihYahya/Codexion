#include "main.h"
struct s_SchedulerState {

};


int init_scheduler(char *str)
{
    int err;

    err = 0;
    if (strcmp(str, "fifo"))
    {
        err = fifo_scheduler();
    }
    else if (strcmp(str, "edf"))
    {
        err = edf_scheduler();
    }
    else{
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