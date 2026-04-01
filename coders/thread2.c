/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 03:54:04 by alone             #+#    #+#             */
/*   Updated: 2026/04/01 15:38:23 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"


static int safe_sleep(struct s_CoderState *s_arg, size_t ms)
{
    size_t elapsed;

    elapsed = 0;
    while (elapsed < ms)
    {
        if (check_death(s_arg) != 0)
            return (-1);
        usleep(1000);
        elapsed++;
    }
    return (0);
}

static int wait_for_my_turn(struct s_scheduler *sch, int my_id, struct s_CoderState *s_arg)
{
    struct s_fifo_queue *q;

    if (!sch || !sch->data)
        return (-1);
    if (pthread_mutex_lock(&sch->sched_lock) != 0)
        return (-1);
    q = (struct s_fifo_queue *)sch->data;

    while (!q->front || q->front->curr_id != my_id)
    {
        if (check_death(s_arg) != 0)  // ← check death while waiting
        {
            pthread_mutex_unlock(&sch->sched_lock);
            return (-1);
        }
        if (pthread_cond_wait(&sch->sched_id, &sch->sched_lock) != 0)
        {
            pthread_mutex_unlock(&sch->sched_lock);
            return (-1);
        }
    }
    if (pthread_mutex_unlock(&sch->sched_lock) != 0)
        return (-1);
    return (0);
}

static int run_compile_round(struct s_CoderState *s_arg, struct s_scheduler *sch)
{
    if (take_usb(s_arg, 0) != 0)
        return (-1);
    if (take_usb(s_arg, 1) != 0)
        return (put_usb(s_arg, 0), -1);
    s_arg->arg->last_time_comp = get_curr_time_ms();
    SAFE_PRINT((struct s_globalstate *)s_arg->gconfig,
        "%zu %u is compiling\n",
        get_curr_time_ms() - s_arg->gconfig->start_time_ms,
        s_arg->id + 1);
    if (safe_sleep(s_arg, s_arg->gconfig->pconfig->time_to_compile) != 0) // ← no more raw usleep
        return (-1);
    s_arg->state = DEBUGGING;
    s_arg->arg->last_time_debug = get_curr_time_ms();
    SAFE_PRINT((struct s_globalstate *)s_arg->gconfig,
        "%zu %u is debugging\n",
        get_curr_time_ms() - s_arg->gconfig->start_time_ms,
        s_arg->id + 1);
    if (put_usb(s_arg, 1) != 0 || put_usb(s_arg, 0) != 0)
        return (-1);
    sch->task_finished(sch, s_arg->id);
    s_arg->is_queued = 0;
    if (pthread_cond_broadcast(&sch->sched_id) != 0)
        return (-1);
    return (0);
}



void coder_thread_comp(struct s_CoderState *s_arg)
{
    struct s_scheduler *sch;

    sch = (struct s_scheduler *)s_arg->gconfig->scheduler;
    if (!s_arg->is_queued)
    {
        if (sch->add_task(sch, s_arg->id, 0) == 0)
            s_arg->is_queued = 1;
        else
            return ;
    }
    if (wait_for_my_turn(s_arg->gconfig->scheduler, s_arg->id, s_arg) != 0)
    {
        sch->task_finished(sch, s_arg->id); // ← free the node before exiting
        s_arg->is_queued = 0;
        return ;
    }
    if (run_compile_round(s_arg, sch) != 0)
        return ;
}

int  coder_thread_debug(struct s_CoderState *s_arg)
{
    if (check_time(s_arg))
        {
            s_arg->state = REFACTOR;
            s_arg->arg->last_time_refact = get_curr_time_ms();
            SAFE_PRINT((struct s_globalstate *)s_arg->gconfig,
                "%zu %u is refactoring\n",
                get_curr_time_ms() - s_arg->gconfig->start_time_ms,
                s_arg->id + 1);
            return (1);
        }
        if (safe_sleep(s_arg, s_arg->gconfig->pconfig->time_to_debug) != 0)
            return (-1);
    return (0);
}

int  coder_thread_refactor(struct s_CoderState *s_arg)
{
    if (check_time(s_arg))
            {
                s_arg->arg->compiled_count++;
                if (s_arg->arg->compiled_count >= 
                    s_arg->gconfig->pconfig->number_of_compiles_required)
                {
                    pthread_mutex_lock(&(s_arg->mstate->death_lock));
                    s_arg->mstate->finished_coders++;
                    pthread_mutex_unlock(&(s_arg->mstate->death_lock));
                    DEBUG("Finished Required Compiles Count");
                    return (-1);
                }
                s_arg->state = COMPILE;
            }
            if (safe_sleep(s_arg, s_arg->gconfig->pconfig->time_to_refactor) != 0)
                return (-1);
    return (0);
}