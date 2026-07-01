/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_random.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/01 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/07/01 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

#ifndef USE_CONCURRENT

/*
** Old-model random scheduler. Coders are held on a singly linked list. When a
** compile slot opens (pick_id < 0), pick_next walks to a random node and pins
** it as the winner; every waiter keeps seeing that same id until it finishes,
** so exactly one coder compiles at a time -- same invariant as fifo/edf.
** task_finished splices the node out (reconnect) and, if it was the winner,
** clears the pin so the next round draws a fresh victim. xorshift keeps us off
** rand() (not an allowed function) and the seed lives in the queue (no global).
*/
static unsigned int	rng_next(unsigned int *state)
{
	unsigned int	x;

	x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*state = x;
	return (x);
}

static int	rand_add_task(struct s_scheduler *sch, int id, long deadline)
{
	struct s_rand_queue	*q;
	struct s_node		*node;

	(void)deadline;
	if (!sch || !sch->data)
		return (-1);
	node = malloc(sizeof(struct s_node));
	if (!node)
		return (-1);
	node->curr_id = id;
	pthread_mutex_lock(&sch->sched_lock);
	q = (struct s_rand_queue *)sch->data;
	node->next = q->head;
	q->head = node;
	q->count++;
	pthread_mutex_unlock(&sch->sched_lock);
	return (0);
}

static void	rand_task_finished(struct s_scheduler *sch, int id)
{
	struct s_rand_queue	*q;
	struct s_node		**pp;
	struct s_node		*tmp;

	if (!sch || !sch->data)
		return ;
	pthread_mutex_lock(&sch->sched_lock);
	q = (struct s_rand_queue *)sch->data;
	pp = &q->head;
	while (*pp && (*pp)->curr_id != id)
		pp = &(*pp)->next;
	if (*pp)
	{
		tmp = *pp;
		*pp = tmp->next;
		free(tmp);
		q->count--;
		if (q->pick_id == id)
			q->pick_id = -1;
	}
	pthread_mutex_unlock(&sch->sched_lock);
}

static int	rand_pick_next(struct s_scheduler *sch, int *out_id, long *out_dl)
{
	struct s_rand_queue	*q;
	struct s_node		*cur;
	unsigned int		steps;

	q = (struct s_rand_queue *)sch->data;
	if (!q || !q->head)
		return (-1);
	if (q->pick_id < 0)
	{
		steps = rng_next(&q->seed) % (unsigned int)q->count;
		cur = q->head;
		while (steps-- > 0)
			cur = cur->next;
		q->pick_id = cur->curr_id;
	}
	*out_id = q->pick_id;
	*out_dl = 0;
	return (0);
}

int	init_random_scheduler(struct s_globalstate *gstate)
{
	struct s_rand_queue	*q;

	q = malloc(sizeof(struct s_rand_queue));
	if (!q)
		return (put_error("Error: alloc random queue failed"), -1);
	q->head = NULL;
	q->count = 0;
	q->pick_id = -1;
	q->seed = (unsigned int)get_curr_time_ms() | 1u;
	gstate->scheduler->data = q;
	gstate->scheduler->add_task = rand_add_task;
	gstate->scheduler->task_finished = rand_task_finished;
	gstate->scheduler->remove_task = NULL;
	gstate->scheduler->pick_next = rand_pick_next;
	return (0);
}

#endif
