/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

#ifndef USE_CONCURRENT

/*
** Original, serialized model (kept as a reference; the concurrent model in
** thread_conc.c is the default, see USE_CONCURRENT in main.h).
**
** Idea: one shared scheduler hands out a single compile turn at a time, so only
** ever one coder holds dongles. Debugging and refactoring are unaffected and
** still overlap freely across coders -- only the compile phase is serialized.
**
** Pros: dead simple to reason about and defend. There is a single ordering
** point, so the FIFO/EDF order is global and exact, and it is deadlock- and
** starvation-free almost by construction (no hold-and-wait, only one critical
** section). Great for small rings, for demoing the scheduling order, or when
** you genuinely want compiles to happen one-by-one.
**
** Cons: the compile phase is the bottleneck -- only one coder compiles at a
** time. A coder waits about number_of_coders * time_to_compile for its turn and
** burns out once time_to_burnout is not much larger than that, so it does not
** scale to many coders or tight deadlines.
**
** The concurrent model trades this simplicity for throughput: non-adjacent
** coders compile in parallel and each dongle arbitrates its own waiters, which
** is what survives realistic parameters and large rings. Prefer it for anything
** that must keep every coder alive under load.
*/
static int	wait_for_my_turn(struct s_scheduler *sch, int my_id,
		struct s_coder *s_arg)
{
	int				front_id;
	long			dl;
	struct timespec	ts;

	if (!sch || !sch->pick_next)
		return (-1);
	if (pthread_mutex_lock(&sch->sched_lock) != 0)
		return (-1);
	front_id = -1;
	while (front_id != my_id)
	{
		sch->pick_next(sch, &front_id, &dl);
		if (front_id == my_id)
			break ;
		if (check_death(s_arg) != 0)
			return (pthread_mutex_unlock(&sch->sched_lock), -1);
		if (get_abs_timeout_from_now_ms(5, &ts) == 0)
			pthread_cond_timedwait(&sch->sched_id, &sch->sched_lock, &ts);
	}
	return (pthread_mutex_unlock(&sch->sched_lock));
}

static int	run_compile_round(struct s_coder *s_arg, struct s_scheduler *sch)
{
	if (take_usb(s_arg, 0) != 0)
		return (-1);
	if (take_usb(s_arg, 1) != 0)
		return (put_usb(s_arg, 0), -1);
	pthread_mutex_lock(&(s_arg->mstate->death_lock));
	s_arg->arg->last_time_comp = get_curr_time_ms();
	pthread_mutex_unlock(&(s_arg->mstate->death_lock));
	log_state((struct s_globalstate *)s_arg->gconfig,
		get_curr_time_ms() - s_arg->gconfig->start_time_ms,
		s_arg->id + 1, "is compiling");
	if (safe_sleep(s_arg, s_arg->gconfig->pconfig->time_to_compile) != 0)
		return (put_usb(s_arg, 1), put_usb(s_arg, 0), -1);
	s_arg->state = DEBUGGING;
	s_arg->arg->last_time_debug = get_curr_time_ms();
	log_state((struct s_globalstate *)s_arg->gconfig,
		get_curr_time_ms() - s_arg->gconfig->start_time_ms,
		s_arg->id + 1, "is debugging");
	put_usb(s_arg, 1);
	put_usb(s_arg, 0);
	sch->task_finished(sch, s_arg->id);
	s_arg->is_queued = 0;
	pthread_cond_broadcast(&sch->sched_id);
	return (0);
}

void	coder_thread_comp(struct s_coder *s_arg)
{
	struct s_scheduler	*sch;
	long				deadline;

	sch = (struct s_scheduler *)s_arg->gconfig->scheduler;
	if (!s_arg->is_queued)
	{
		deadline = (long)(s_arg->arg->last_time_comp
				+ s_arg->gconfig->pconfig->time_to_burnout);
		if (sch->add_task(sch, s_arg->id, deadline) != 0)
			return ;
		s_arg->is_queued = 1;
	}
	if (wait_for_my_turn(sch, s_arg->id, s_arg) != 0)
	{
		sch->task_finished(sch, s_arg->id);
		s_arg->is_queued = 0;
		return ;
	}
	run_compile_round(s_arg, sch);
}

#endif
