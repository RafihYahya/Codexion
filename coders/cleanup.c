/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 03:54:40 by alone             #+#    #+#             */
/*   Updated: 2026/04/05 14:22:58 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"


static void cleanup_coder_and_usb(struct s_globalstate *gstate, size_t num_coders)
{
    size_t i;

    i = 0;
    if (gstate->cstates)
    {
        while (i < num_coders)
            free(gstate->cstates[i++].arg);
        free(gstate->cstates);
    }
    if (gstate->states)
    {
        i = 0;
        while (i < num_coders)
        {
            pthread_mutex_destroy(&(gstate->states[i].usb_mutex));
            pthread_cond_destroy(&(gstate->states[i].usb_rec_cond));
            i++;
        }
        free(gstate->states);
    }
}

static void cleanup_scheduler(struct s_scheduler *scheduler)
{
    struct s_fifo_queue *fifo_q;
    struct s_node       *curr;
    struct s_node       *next;

    if (!scheduler)
        return ;
    if (scheduler->data)
    {
        fifo_q = (struct s_fifo_queue *)scheduler->data;
        curr = fifo_q->front;
        while (curr)           //free all remaining nodes first
        {
            next = curr->next;
            free(curr);
            curr = next;
        }
        free(scheduler->data);
    }
    pthread_mutex_destroy(&(scheduler->sched_lock));
    pthread_cond_destroy(&(scheduler->sched_id));
    free(scheduler);
}

void mem_cleanup(struct s_globalstate *gstate)
{
    size_t num_coders;

    if (!gstate)
        return ;
    num_coders = 0;
    if (gstate->pconfig)
        num_coders = gstate->pconfig->number_of_coders;
    pthread_mutex_destroy(&gstate->print_lock);
    if (gstate->mstate)
    {
        pthread_mutex_destroy(&gstate->mstate->death_lock);
        free(gstate->mstate);
    }
    cleanup_coder_and_usb(gstate, num_coders);
    free(gstate->thd);
    cleanup_scheduler(gstate->scheduler);
    free((void *)gstate->pconfig);
}