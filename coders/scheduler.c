/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/30 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

#ifndef USE_CONCURRENT

static int	init_scheduler_sync(struct s_scheduler *sch)
{
	if (pthread_mutex_init(&(sch->sched_lock), NULL) != 0)
		return (-1);
	if (pthread_cond_init(&(sch->sched_id), NULL) != 0)
	{
		pthread_mutex_destroy(&(sch->sched_lock));
		return (-1);
	}
	sch->data = NULL;
	return (0);
}

static void	free_edf_data(struct s_scheduler *sch)
{
	struct s_edf_node	*cur;
	struct s_edf_node	*next;

	cur = (struct s_edf_node *)sch->data;
	while (cur)
	{
		next = cur->next;
		free(cur);
		cur = next;
	}
}

static void	free_fifo_data(struct s_scheduler *sch)
{
	struct s_fifo_queue	*q;
	struct s_node		*curr;
	struct s_node		*next;

	if (!sch->data)
		return ;
	q = (struct s_fifo_queue *)sch->data;
	curr = q->front;
	while (curr)
	{
		next = curr->next;
		free(curr);
		curr = next;
	}
	free(q);
}

void	cleanup_sched(struct s_globalstate *g)
{
	if (!g->scheduler)
		return ;
	if (g->pconfig && strcmp(g->pconfig->scheduler, "edf") == 0)
		free_edf_data(g->scheduler);
	else
		free_fifo_data(g->scheduler);
	pthread_mutex_destroy(&(g->scheduler->sched_lock));
	pthread_cond_destroy(&(g->scheduler->sched_id));
	free(g->scheduler);
}

int	init_scheduler(struct s_globalstate *gstate)
{
	if (!gstate)
		return (-1);
	gstate->scheduler = malloc(sizeof(struct s_scheduler));
	if (!(gstate->scheduler))
		return (put_error("Error: malloc scheduler failed"), -1);
	if (init_scheduler_sync(gstate->scheduler) != 0)
	{
		free(gstate->scheduler);
		gstate->scheduler = NULL;
		return (put_error("Error: init scheduler sync failed"), -1);
	}
	if (strcmp(gstate->pconfig->scheduler, "fifo") == 0)
		return (init_fifo_scheduler(gstate));
	if (strcmp(gstate->pconfig->scheduler, "edf") == 0)
		return (init_edf_scheduler(gstate));
	return (put_error("Error: invalid scheduler"), -1);
}

#endif
