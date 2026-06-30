/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/30 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static void	free_waitq(struct s_usb *d)
{
	struct s_req	*cur;
	struct s_req	*next;

	cur = d->waitq;
	while (cur)
	{
		next = cur->next;
		free(cur);
		cur = next;
	}
	d->waitq = NULL;
}

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
			free_waitq(&gstate->states[i]);
			pthread_mutex_destroy(&(gstate->states[i].usb_mutex));
			pthread_cond_destroy(&(gstate->states[i].usb_rec_cond));
			i++;
		}
		free(gstate->states);
	}
}

void	mem_cleanup(struct s_globalstate *gstate)
{
	size_t	num_coders;

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
	cleanup_sched(gstate);
	free((void *)gstate->pconfig);
}
