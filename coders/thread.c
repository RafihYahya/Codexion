#include "main.h"


void *coder_thread(void *arg)
{
    struct s_CoderState *s_arg;

    s_arg = (struct s_CoderState *)arg;

    DEBUG("Inside Coder Thread");
    while(1)
    {
        pthread_mutex_lock(&(s_arg->mstate->death_lock));
        if (s_arg->mstate->is_someone_dead == 1)
        {
            pthread_mutex_unlock(&(s_arg->mstate->death_lock));
            RUN_TEST(
                if (s_arg->id == 0)
                {
                    DEBUG("Someone Died, Time to Join Coder %d", s_arg->id);
                }
            );
            return (NULL);
        }
        pthread_mutex_unlock(&(s_arg->mstate->death_lock));

        RUN_TEST(
            pthread_mutex_lock(&(s_arg->mstate->death_lock));
            s_arg->mstate->is_someone_dead = 1;
            pthread_mutex_unlock(&(s_arg->mstate->death_lock));
        );

        if (s_arg->state == COMPILE)
        {
            if(get_curr_time_ms() - s_arg->arg->last_time_comp >= s_arg->gconfig->pconfig->time_to_compile)
            {
                s_arg->state = DEBUGGING;
            }
        }
        else if (s_arg->state == DEBUGGING)
        {

        }
        else if (s_arg->state == REFACTOR)
        {

        }
        else
        {
            
        }
        // maybe sleep a bit
        usleep(1000);
    };
    DEBUG("End of Coder Thread");
    return (NULL);
}


int init_coder_threads(struct s_globalstate *arg, struct s_CoderState **cstates,
    struct s_UsbDongleState **mutexes,  pthread_t **thd)
{
    size_t                i;
    size_t                j;
    struct s_CoderArg *carg;

    i = 0;
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
    if (!*thd || !*cstates)
    {
        ERROR("Failed Allocation");
        return (-1);
    }
    while (i < arg->pconfig->number_of_coders)
    {
        carg = malloc(sizeof(struct s_CoderArg));
        if (!carg)
        {
            ERROR("Failed Allocation");
            j = 0;
            while (j < i)
                free((*cstates)[j++].arg);
            return (free(*thd), free(*cstates), -1);
        }
        carg->compiled_count = 0;
        carg->last_time_comp = 0;
        carg->last_time_debug = 0;
        carg->last_time_refact = 0;
        (*cstates)[i].id = i;
        (*cstates)[i].gconfig = arg;
        (*cstates)[i].state = COMPILE;
        (*cstates)[i].mstate = arg->mstate;
        (*cstates)[i].l_usb = &(*mutexes)[i % arg->pconfig->number_of_coders];
        (*cstates)[i].r_usb = &(*mutexes)[(i + 1) % arg->pconfig->number_of_coders]; // not sure if this is correct
        (*cstates)[i].arg = carg;
        if (pthread_create(&(*thd)[i], NULL, coder_thread, &(*cstates)[i]) != 0)
        {
            ERROR("Failed creation of threads");
            return (free(*thd), free(*cstates), -1);
        }
        i++;
    }
    DEBUG("End of Thread Init function");
    return (0);
}