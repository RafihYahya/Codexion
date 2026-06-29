/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

size_t	get_curr_time_ms(void)
{
	struct timeval	tv;
	size_t			ms;

	if (gettimeofday(&tv, NULL) != 0)
	{
		put_error("Error: can't get the time");
		return (-1);
	}
	ms = (size_t)tv.tv_sec * 1000 + (size_t)tv.tv_usec / 1000;
	return (ms);
}

size_t	check_time(struct s_coder *s_arg)
{
	size_t	curr_time;

	curr_time = get_curr_time_ms();
	if (s_arg->state == COMPILE)
		return (curr_time - s_arg->arg->last_time_comp
			>= s_arg->gconfig->pconfig->time_to_compile);
	if (s_arg->state == DEBUGGING)
		return (curr_time - s_arg->arg->last_time_debug
			>= s_arg->gconfig->pconfig->time_to_debug);
	if (s_arg->state == REFACTOR)
		return (curr_time - s_arg->arg->last_time_refact
			>= s_arg->gconfig->pconfig->time_to_refactor);
	return (0);
}

size_t	get_abs_timeout_from_now_ms(size_t wait_ms, struct timespec *ts)
{
	struct timeval	tv;
	size_t			usec_total;

	if (gettimeofday(&tv, NULL) != 0)
		return (-1);
	ts->tv_sec = tv.tv_sec + (time_t)(wait_ms / 1000);
	usec_total = (size_t)tv.tv_usec + (wait_ms % 1000) * 1000;
	ts->tv_sec += (time_t)(usec_total / 1000000);
	ts->tv_nsec = (long)((usec_total % 1000000) * 1000);
	return (0);
}
