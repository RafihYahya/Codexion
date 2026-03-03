/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alone <alone@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 03:54:08 by alone             #+#    #+#             */
/*   Updated: 2026/03/03 04:40:12 by alone            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"


static int check_death(struct s_CoderState *s_arg)
{
    pthread_mutex_lock(&(s_arg->mstate->death_lock));
        if (s_arg->mstate->is_someone_dead == 1)
        {
            pthread_mutex_unlock(&(s_arg->mstate->death_lock));
            RUN_TEST(
                if (s_arg->id == 0)
                {
                    DEBUG("Someone Died, Time to Join Coder %d", s_arg->id);
                    return (-1);
                }
            );
            return (-1);
        }
    pthread_mutex_unlock(&(s_arg->mstate->death_lock));
    return (0);
}

static int check_burnout(struct s_CoderState *s_arg)
{
        if (s_arg->state == COMPILE &&
            get_curr_time_ms() - s_arg->arg->last_time_comp >=
            (size_t)s_arg->gconfig->pconfig->time_to_burnout)
        {
            pthread_mutex_lock(&(s_arg->mstate->death_lock));
            s_arg->mstate->is_someone_dead = 1;
            pthread_mutex_unlock(&(s_arg->mstate->death_lock));
            SAFE_PRINT((struct s_globalstate *)s_arg->gconfig,
                "%zu %u burned out\n",
                get_curr_time_ms() - s_arg->gconfig->start_time_ms,
                s_arg->id + 1);
            return (-1);
        }
    return (0);
}

void *coder_thread(void *arg)
{
    struct s_CoderState *s_arg;

    s_arg = (struct s_CoderState *)arg;
    DEBUG("Inside Coder Thread");
    while(1)
    {
        if (check_death(s_arg) != 0)
            return (NULL);
        if (check_burnout(s_arg) != 0)
            return (NULL);
        if (s_arg->state == COMPILE)
            coder_thread_comp(s_arg);
        else if (s_arg->state == DEBUGGING && coder_thread_debug(s_arg) == 1)
            continue;
        else if (s_arg->state == REFACTOR && coder_thread_refactor(s_arg) != 0)
            return (NULL);
        usleep(1000);
    };
    DEBUG("End of Coder Thread");
    return (NULL);
}