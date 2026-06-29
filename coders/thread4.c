/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread4.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	coder_thread_debug(struct s_coder *s_arg)
{
	if (!check_time(s_arg))
		return (safe_sleep(s_arg, s_arg->gconfig->pconfig->time_to_debug));
	s_arg->state = REFACTOR;
	s_arg->arg->last_time_refact = get_curr_time_ms();
	log_state((struct s_globalstate *)s_arg->gconfig,
		get_curr_time_ms() - s_arg->gconfig->start_time_ms,
		s_arg->id + 1, "is refactoring");
	return (1);
}

int	coder_thread_refactor(struct s_coder *s_arg)
{
	if (!check_time(s_arg))
		return (safe_sleep(s_arg, s_arg->gconfig->pconfig->time_to_refactor));
	pthread_mutex_lock(&(s_arg->mstate->death_lock));
	s_arg->arg->compiled_count++;
	if (s_arg->arg->compiled_count
		>= s_arg->gconfig->pconfig->number_of_compiles_required)
	{
		s_arg->mstate->finished_coders++;
		pthread_mutex_unlock(&(s_arg->mstate->death_lock));
		return (-1);
	}
	pthread_mutex_unlock(&(s_arg->mstate->death_lock));
	s_arg->state = COMPILE;
	return (0);
}
