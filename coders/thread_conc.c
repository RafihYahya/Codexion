/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread_conc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/30 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

#ifdef USE_CONCURRENT

/*
** Even and odd coders pick up their two dongles in opposite orders. This
** asymmetry makes the circular-wait Coffman condition impossible (so no
** deadlock) while keeping hold-and-wait chains local, which avoids the
** startup convoy that strict index ordering would create on large rings.
*/
static int	run_compile_round(struct s_coder *s_arg, struct s_usb *first,
		struct s_usb *second)
{
	if (take_dongle(s_arg, first) != 0)
		return (-1);
	if (take_dongle(s_arg, second) != 0)
		return (put_dongle(s_arg, first), -1);
	pthread_mutex_lock(&(s_arg->mstate->death_lock));
	s_arg->arg->last_time_comp = get_curr_time_ms();
	pthread_mutex_unlock(&(s_arg->mstate->death_lock));
	log_state((struct s_globalstate *)s_arg->gconfig,
		get_curr_time_ms() - s_arg->gconfig->start_time_ms,
		s_arg->id + 1, "is compiling");
	if (safe_sleep(s_arg, s_arg->gconfig->pconfig->time_to_compile) != 0)
		return (put_dongle(s_arg, second), put_dongle(s_arg, first), -1);
	s_arg->state = DEBUGGING;
	s_arg->arg->last_time_debug = get_curr_time_ms();
	log_state((struct s_globalstate *)s_arg->gconfig,
		get_curr_time_ms() - s_arg->gconfig->start_time_ms,
		s_arg->id + 1, "is debugging");
	put_dongle(s_arg, second);
	put_dongle(s_arg, first);
	return (0);
}

void	coder_thread_comp(struct s_coder *s_arg)
{
	if (s_arg->id % 2 == 0)
		run_compile_round(s_arg, s_arg->l_usb, s_arg->r_usb);
	else
		run_compile_round(s_arg, s_arg->r_usb, s_arg->l_usb);
}

#endif
