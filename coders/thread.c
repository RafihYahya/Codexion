#include "thread.h"

void *coder_thread(void *arg)
{

}


int init_coder_threads(struct s_ArgvParsedConfig *arg, struct s_CoderState *cstates, pthread_t *thd)
{
    int                i;
    int                err;
    struct s_CoderArg *carg;

    i = 0;
    err = 0;
    carg = NULL;
    DEBUG("Creation of Coder Threads along with their corresponding states");
    if (!arg || cstates || thd)
    {
        ERROR("Arguments Invalid");
        return (-1);
    }
    DEBUG("Setting up states and thread allocations");
    cstates = malloc(sizeof(struct s_CoderState) * arg->number_of_coders);
    thd = malloc(sizeof(pthread_t) * arg->number_of_coders);
    if (!thd || !cstates)
    {
        ERROR("Failed Allocation");
        return (-1);
    }
    while (i < arg->number_of_coders)
    {
        err = pthread_create(thd[i], NULL, coder_thread, &cstates[i]);
        if (err < 0)
        {
            ERROR("Failed creation of threads");
            return (free(thd),free(cstates), -1);
        }
        carg = malloc(sizeof(struct s_CoderArg));
        if (!carg)
        {
            ERROR("Failed Allocation");
            return (free(thd), free(cstates), -1);
        }
        cstates[i].id = i;
        cstates[i].gconfig = arg;
        cstates[i].state = STANDBY;
        cstates[i].l_usb = NULL;
        cstates[i].r_usb = NULL;
        cstates[i].arg = carg;
    }
    DEBUG("End of Thread Init function");
    return (0);
}