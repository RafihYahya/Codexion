/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_fifo.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/30 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

#ifndef USE_CONCURRENT

static void	remove_node(struct s_fifo_queue *fifo_q, struct s_node *prev,
		struct s_node *curr)
{
	if (prev)
		prev->next = curr->next;
	else
		fifo_q->front = curr->next;
	if (fifo_q->back == curr)
		fifo_q->back = prev;
	if (fifo_q->count > 0)
		fifo_q->count--;
	free(curr);
}

static void	fifo_task_finished(struct s_scheduler *sch, int id)
{
	struct s_fifo_queue	*fifo_q;
	struct s_node		*curr;
	struct s_node		*prev;

	if (!sch || !sch->data)
		return ;
	pthread_mutex_lock(&sch->sched_lock);
	fifo_q = (struct s_fifo_queue *)sch->data;
	curr = fifo_q->front;
	prev = NULL;
	while (curr)
	{
		if (curr->curr_id == id)
		{
			remove_node(fifo_q, prev, curr);
			break ;
		}
		prev = curr;
		curr = curr->next;
	}
	pthread_mutex_unlock(&sch->sched_lock);
}

static int	fifo_add_task(struct s_scheduler *sch, int id, long deadline)
{
	struct s_fifo_queue	*fifo_q;
	struct s_node		*new_node;

	(void)deadline;
	if (!sch || !sch->data)
		return (-1);
	new_node = malloc(sizeof(struct s_node));
	if (!new_node)
		return (-1);
	new_node->curr_id = id;
	new_node->next = NULL;
	pthread_mutex_lock(&sch->sched_lock);
	fifo_q = (struct s_fifo_queue *)sch->data;
	if (!fifo_q->front)
		fifo_q->front = new_node;
	else
		fifo_q->back->next = new_node;
	fifo_q->back = new_node;
	fifo_q->count++;
	pthread_mutex_unlock(&sch->sched_lock);
	return (0);
}

static int	fifo_pick_next(struct s_scheduler *sch, int *out_id, long *out_dl)
{
	struct s_fifo_queue	*fifo_q;

	fifo_q = (struct s_fifo_queue *)sch->data;
	if (!fifo_q || !fifo_q->front)
		return (-1);
	*out_id = fifo_q->front->curr_id;
	*out_dl = 0;
	return (0);
}

int	init_fifo_scheduler(struct s_globalstate *gstate)
{
	struct s_fifo_queue	*fifo_q;

	gstate->scheduler->data = malloc(sizeof(struct s_fifo_queue));
	if (!gstate->scheduler->data)
		return (put_error("Error: alloc fifo queue failed"), -1);
	fifo_q = (struct s_fifo_queue *)gstate->scheduler->data;
	fifo_q->front = NULL;
	fifo_q->back = NULL;
	fifo_q->count = 0;
	gstate->scheduler->add_task = fifo_add_task;
	gstate->scheduler->task_finished = fifo_task_finished;
	gstate->scheduler->remove_task = NULL;
	gstate->scheduler->pick_next = fifo_pick_next;
	return (0);
}

#endif
