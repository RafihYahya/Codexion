/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static void	init_gstate(struct s_globalstate *gstate)
{
	gstate->pconfig = NULL;
	gstate->states = NULL;
	gstate->cstates = NULL;
	gstate->mstate = NULL;
	gstate->scheduler = NULL;
	gstate->thd = NULL;
	gstate->start_time_ms = 0;
}

static int	setup_resources(struct s_globalstate *gstate, int argc, char **argv)
{
	gstate->pconfig = create_config(argc, argv);
	if (!gstate->pconfig)
		return (-1);
	if (init_usb_mutexes_conds(gstate->pconfig->number_of_coders,
			&(gstate->states)) < 0)
		return (-1);
	if (init_scheduler(gstate) < 0)
		return (-1);
	return (0);
}

static void	stop_and_join_coders(struct s_globalstate *gstate)
{
	size_t	k;

	pthread_mutex_lock(&gstate->mstate->death_lock);
	gstate->mstate->is_someone_dead = 1;
	pthread_mutex_unlock(&gstate->mstate->death_lock);
	k = 0;
	while (k < gstate->pconfig->number_of_coders)
		pthread_join(gstate->thd[k++], NULL);
}

static int	run_simulation(struct s_globalstate *gstate)
{
	size_t	k;

	gstate->start_time_ms = get_curr_time_ms();
	if (init_monitor_state(gstate) < 0)
		return (-1);
	if (init_coder_threads(gstate) < 0)
		return (-1);
	if (start_monitor(gstate) < 0)
		return (stop_and_join_coders(gstate), -1);
	pthread_join(gstate->mstate->monitor, NULL);
	k = 0;
	while (k < gstate->pconfig->number_of_coders)
		pthread_join(gstate->thd[k++], NULL);
	return (0);
}

int	main(int argc, char **argv)
{
	struct s_globalstate	gstate;

	init_gstate(&gstate);
	if (pthread_mutex_init(&(gstate.print_lock), NULL) != 0)
		return (1);
	if (setup_resources(&gstate, argc, argv) < 0
		|| run_simulation(&gstate) < 0)
	{
		mem_cleanup(&gstate);
		return (1);
	}
	mem_cleanup(&gstate);
	return (0);
}
