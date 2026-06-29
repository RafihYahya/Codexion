/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	check_one_burnout(struct s_globalstate *g, size_t i, size_t now)
{
	struct s_coder_arg	*a;

	a = g->cstates[i].arg;
	if (!a || a->compiled_count >= g->pconfig->number_of_compiles_required)
		return (0);
	if (now - a->last_time_comp < g->pconfig->time_to_burnout)
		return (0);
	return (1);
}

static int	scan_state(struct s_globalstate *g)
{
	size_t	i;
	size_t	now;

	if (g->mstate->is_someone_dead)
		return (-3);
	if (g->mstate->finished_coders >= g->pconfig->number_of_coders)
		return (-2);
	now = get_curr_time_ms();
	i = 0;
	while (i < g->pconfig->number_of_coders)
	{
		if (check_one_burnout(g, i, now))
		{
			g->mstate->is_someone_dead = 1;
			return ((int)i);
		}
		i++;
	}
	return (-1);
}

static void	*monitor_thread(void *arg)
{
	struct s_globalstate	*g;
	int						r;

	g = (struct s_globalstate *)arg;
	while (1)
	{
		pthread_mutex_lock(&g->mstate->death_lock);
		r = scan_state(g);
		pthread_mutex_unlock(&g->mstate->death_lock);
		if (r == -2 || r == -3)
			return (NULL);
		if (r >= 0)
		{
			log_state(g, get_curr_time_ms() - g->start_time_ms,
				(unsigned int)r + 1, "burned out");
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

int	init_monitor_state(struct s_globalstate *gstate)
{
	gstate->mstate = malloc(sizeof(struct s_monitorstate));
	if (!gstate->mstate)
		return (-1);
	if (pthread_mutex_init(&(gstate->mstate->death_lock), NULL) != 0)
		return (free(gstate->mstate), gstate->mstate = NULL, -1);
	gstate->mstate->is_someone_dead = 0;
	gstate->mstate->finished_coders = 0;
	return (0);
}

int	start_monitor(struct s_globalstate *gstate)
{
	if (pthread_create(&(gstate->mstate->monitor), NULL,
			monitor_thread, gstate) != 0)
		return (put_error("Error: failed creating monitor thread"), -1);
	return (0);
}
