/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static void	cleanup_coder_and_usb(struct s_globalstate *gstate, size_t n)
{
	size_t	i;

	i = 0;
	if (gstate->cstates)
	{
		while (i < n)
			free(gstate->cstates[i++].arg);
		free(gstate->cstates);
	}
	if (gstate->states)
	{
		i = 0;
		while (i < n)
		{
			pthread_mutex_destroy(&(gstate->states[i].usb_mutex));
			pthread_cond_destroy(&(gstate->states[i].usb_rec_cond));
			i++;
		}
		free(gstate->states);
	}
}

static void	free_fifo_data(struct s_scheduler *sch)
{
	struct s_fifo_queue	*fifo_q;
	struct s_node		*curr;
	struct s_node		*next;

	if (!sch->data)
		return ;
	fifo_q = (struct s_fifo_queue *)sch->data;
	curr = fifo_q->front;
	while (curr)
	{
		next = curr->next;
		free(curr);
		curr = next;
	}
	free(sch->data);
}

static void	free_edf_data(struct s_scheduler *sch)
{
	struct s_edf_node	*curr;
	struct s_edf_node	*next;

	curr = (struct s_edf_node *)sch->data;
	while (curr)
	{
		next = curr->next;
		free(curr);
		curr = next;
	}
}

static void	cleanup_scheduler(struct s_scheduler *sch, const char *type)
{
	if (!sch)
		return ;
	if (type && strcmp(type, "edf") == 0)
		free_edf_data(sch);
	else
		free_fifo_data(sch);
	pthread_mutex_destroy(&(sch->sched_lock));
	pthread_cond_destroy(&(sch->sched_id));
	free(sch);
}

void	mem_cleanup(struct s_globalstate *gstate)
{
	size_t		num_coders;
	const char	*type;

	if (!gstate)
		return ;
	num_coders = 0;
	type = NULL;
	if (gstate->pconfig)
	{
		num_coders = gstate->pconfig->number_of_coders;
		type = gstate->pconfig->scheduler;
	}
	pthread_mutex_destroy(&gstate->print_lock);
	if (gstate->mstate)
	{
		pthread_mutex_destroy(&gstate->mstate->death_lock);
		free(gstate->mstate);
	}
	cleanup_coder_and_usb(gstate, num_coders);
	free(gstate->thd);
	cleanup_scheduler(gstate->scheduler, type);
	free((void *)gstate->pconfig);
}
