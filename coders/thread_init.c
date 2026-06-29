/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static void	init_one_coder_state(struct s_globalstate *gstate, size_t i,
		struct s_coder_arg *carg)
{
	unsigned int	n;

	n = gstate->pconfig->number_of_coders;
	carg->last_time_comp = get_curr_time_ms();
	carg->last_time_debug = 0;
	carg->last_time_refact = 0;
	carg->compiled_count = 0;
	gstate->cstates[i].id = (unsigned int)i;
	gstate->cstates[i].is_queued = 0;
	gstate->cstates[i].gconfig = gstate;
	gstate->cstates[i].state = COMPILE;
	gstate->cstates[i].mstate = gstate->mstate;
	gstate->cstates[i].l_usb = &gstate->states[i % n];
	gstate->cstates[i].r_usb = &gstate->states[(i + 1) % n];
	gstate->cstates[i].arg = carg;
}

static int	helper_fail(struct s_globalstate *gstate, size_t created)
{
	size_t	j;

	pthread_mutex_lock(&gstate->mstate->death_lock);
	gstate->mstate->is_someone_dead = 1;
	pthread_mutex_unlock(&gstate->mstate->death_lock);
	j = 0;
	while (j < created)
		pthread_join(gstate->thd[j++], NULL);
	return (-1);
}

static int	init_coder_threads_helper(struct s_globalstate *gstate)
{
	size_t				i;
	struct s_coder_arg	*carg;

	i = 0;
	while (i < (size_t)gstate->pconfig->number_of_coders)
	{
		carg = malloc(sizeof(struct s_coder_arg));
		if (!carg)
			return (helper_fail(gstate, i));
		init_one_coder_state(gstate, i, carg);
		if (pthread_create(&gstate->thd[i], NULL, coder_thread,
				&gstate->cstates[i]) != 0)
		{
			free(carg);
			gstate->cstates[i].arg = NULL;
			return (helper_fail(gstate, i));
		}
		i++;
	}
	return (0);
}

int	init_coder_threads(struct s_globalstate *gstate)
{
	size_t	size;

	if (!gstate || !gstate->states || !gstate->pconfig)
		return (-1);
	size = sizeof(struct s_coder) * gstate->pconfig->number_of_coders;
	gstate->cstates = malloc(size);
	if (!gstate->cstates)
		return (put_error("Error: allocation failed"), -1);
	memset(gstate->cstates, 0, size);
	gstate->thd = malloc(sizeof(pthread_t)
			* gstate->pconfig->number_of_coders);
	if (!gstate->thd)
		return (put_error("Error: allocation failed"), -1);
	if (init_coder_threads_helper(gstate) != 0)
		return (-1);
	return (0);
}
