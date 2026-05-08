/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_edf.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 03:54:20 by alone             #+#    #+#             */
/*   Updated: 2026/05/08 15:49:49 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int edf_add_task(struct s_scheduler *sch, int id, long deadline)
{
    struct s_edf_node **head;
    struct s_edf_node *node;
    struct s_edf_node *cur;

    if (!sch || !sch->data)
        return (-1);
    node = malloc(sizeof(struct s_edf_node));
    if (!node)
        return (-1);
    node->curr_id = id;
    node->deadline = deadline;
    node->next = NULL;
    pthread_mutex_lock(&sch->sched_lock);
    head = (struct s_edf_node **)&sch->data;
    cur = *head;
    if (!cur || cur->deadline > deadline)
    {
        node->next = cur;
        *head = node;
    }
    else
    {
        while (cur->next && cur->next->deadline <= deadline)
            cur = cur->next;
        node->next = cur->next;
        cur->next = node;
    }
    pthread_mutex_unlock(&sch->sched_lock);
    return (0);
}

static void edf_task_finished(struct s_scheduler *sch, int id)
{
    struct s_edf_node **head;
    struct s_edf_node *prev;
    struct s_edf_node *cur;

    if (!sch || !sch->data)
        return ;
    pthread_mutex_lock(&sch->sched_lock);
    head = (struct s_edf_node **)&sch->data;
    cur = *head;
    prev = NULL;
    while (cur)
    {
        if (cur->curr_id == id)
        {
            if (prev)
                prev->next = cur->next;
            else
                *head = cur->next;
            free(cur);
            pthread_mutex_unlock(&sch->sched_lock);
            return ;
        }
        prev = cur;
        cur = cur->next;
    }
    pthread_mutex_unlock(&sch->sched_lock);
}

static int edf_remove_task(struct s_scheduler *sch, int id)
{
    edf_task_finished(sch, id);
    return (0);
}

static int edf_pick_next(struct s_scheduler *sch, int *out_id, long *out_deadline)
{
    struct s_edf_node *head;

    if (!sch || !sch->data)
        return (-1);
    head = (struct s_edf_node *)sch->data;
    if (!head)
        return (-1);
    *out_id = head->curr_id;
    *out_deadline = head->deadline;
    return (0);
}

int init_edf_scheduler(struct s_globalstate *gstate)
{
    gstate->scheduler->data = NULL;
    gstate->scheduler->add_task = edf_add_task;
    gstate->scheduler->task_finished = edf_task_finished;
    gstate->scheduler->remove_task = edf_remove_task;
    gstate->scheduler->pick_next = edf_pick_next;
    return (0);
}