#include "main.h"


struct s_scheduler {
    int (*add_task)(struct s_scheduler*, int id, long deadline);
    int (*remove_task)(struct s_scheduler*, int id);
    void (*task_finished)(struct s_scheduler *, int id);
    int (*pick_next)(struct s_scheduler*, int *out_id, long *out_deadline);

    void *data;
};


int init_scheduler(char *str, struct s_globalstate *gstate)
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