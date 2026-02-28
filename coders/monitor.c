#include "main.h"

void *monitor_thread(void *arg)
{
    struct s_globalstate *s_arg;

    s_arg = (struct s_globalstate *)arg;
    DEBUG("Inside Monitor Thread");
    while(1)
    {
        pthread_mutex_lock(&(s_arg->mstate->death_lock));
        if (s_arg->mstate->is_someone_dead == 1)
        {
            DEBUG("Someone Died, Time to Join Monitor");
            pthread_mutex_unlock(&(s_arg->mstate->death_lock));
            return (NULL);
        }
        pthread_mutex_unlock(&(s_arg->mstate->death_lock));
        // maybe sleep here a bit
    };
    DEBUG("End of Monitor Thread");
}

int init_monitor_thread(struct s_globalstate *gstate)
{

    gstate->mstate = malloc(sizeof(struct s_monitorstate));
    if (!gstate->mstate)
        return (-1);
    if (pthread_mutex_init(&(gstate->mstate->death_lock), NULL) != 0)
        return (free(gstate->mstate), -1);
    gstate->mstate->is_someone_dead = 0;
    if (pthread_create(&(gstate->mstate->monitor), NULL, monitor_thread, gstate) != 0 )
    {
        ERROR("Failed Creation of the Thread");
        return (free(gstate->mstate), -1);
    }
    return (0);
}