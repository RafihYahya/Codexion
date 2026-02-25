#include "main.h"


void *coder_thread(void *arg)
{

}


int init_coder_threads(struct s_globalstate *arg, struct s_CoderState **cstates,
    struct s_UsbDongleState **mutexes,  pthread_t **thd)
{
    int                i;
    int                err;
    struct s_CoderArg *carg;

    i = 0;
    err = 0;
    carg = NULL;
    DEBUG("Creation of Coder Threads along with their corresponding states");
    if (!arg || !cstates || !thd || !mutexes)
    {
        ERROR("Arguments Invalid/Null");
        return (-1);
    }
    DEBUG("Setting up states and thread allocations");
    *cstates = malloc(sizeof(struct s_CoderState) * arg->pconfig->number_of_coders);
    *thd = malloc(sizeof(pthread_t) * arg->pconfig->number_of_coders);
    if (!thd || !cstates)
    {
        ERROR("Failed Allocation");
        return (-1);
    }
    while (i < arg->pconfig->number_of_coders)
    {
        err = pthread_create(thd[i], NULL, coder_thread, &cstates[i]);
        if (err < 0)
        {
            ERROR("Failed creation of threads");
            return (free(thd), free(cstates), -1);
        }
        carg = malloc(sizeof(struct s_CoderArg));
        if (!carg)
        {
            ERROR("Failed Allocation");
            int j = 0;
            while (j < i)
                free((*cstates[j++]).arg);
            return (free(thd), free(cstates), -1);
        }
        (*cstates[i]).id = i;
        (*cstates[i]).gconfig = arg;
        (*cstates[i]).state = STANDBY;
        (*cstates[i]).l_usb = &mutexes[i % arg->pconfig->number_of_coders];
        (*cstates[i]).r_usb = &mutexes[(i + 1) % arg->pconfig->number_of_coders]; // not sure if this is correct
        (*cstates[i]).arg = carg;
        i++;
    }
    DEBUG("End of Thread Init function");
    return (0);
}