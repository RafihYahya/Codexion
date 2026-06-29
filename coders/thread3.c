/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	mark_usb_taken(struct s_coder *s_arg, struct s_usb *usb)
{
	usb->is_available = 0;
	log_state((struct s_globalstate *)s_arg->gconfig,
		get_curr_time_ms() - s_arg->gconfig->start_time_ms,
		s_arg->id + 1, "has taken a dongle");
	return (1);
}

static int	wait_usb_cooldown(struct s_coder *s_arg, struct s_usb *usb,
		size_t now_ms)
{
	size_t			ready_ms;
	size_t			wait_ms;
	struct timespec	ts;

	ready_ms = usb->cdown_start + s_arg->gconfig->pconfig->dongle_cooldown;
	if (usb->cdown_start == 0 || now_ms >= ready_ms)
		return (mark_usb_taken(s_arg, usb));
	wait_ms = ready_ms - now_ms;
	if (get_abs_timeout_from_now_ms(wait_ms, &ts) != 0)
		return (-1);
	if (pthread_cond_timedwait(&usb->usb_rec_cond, &usb->usb_mutex, &ts) != 0)
	{
		now_ms = get_curr_time_ms();
		if (now_ms < ready_ms)
			return (-1);
	}
	return (0);
}

static struct s_usb	*get_usb_by_direc(struct s_coder *s_arg, int direc)
{
	if (direc == 0)
		return (s_arg->l_usb);
	return (s_arg->r_usb);
}

int	take_usb(struct s_coder *s_arg, int direc)
{
	struct s_usb	*usb;
	struct timespec	ts;
	int				status;

	usb = get_usb_by_direc(s_arg, direc);
	if (pthread_mutex_lock(&usb->usb_mutex) != 0)
		return (-1);
	while (1)
	{
		if (check_death(s_arg) != 0)
			return (pthread_mutex_unlock(&usb->usb_mutex), -1);
		if (usb->is_available == 1)
		{
			status = wait_usb_cooldown(s_arg, usb, get_curr_time_ms());
			if (status < 0)
				return (pthread_mutex_unlock(&usb->usb_mutex), -1);
			if (status > 0)
				break ;
		}
		else if (get_abs_timeout_from_now_ms(5, &ts) == 0)
			pthread_cond_timedwait(&usb->usb_rec_cond, &usb->usb_mutex, &ts);
	}
	return (pthread_mutex_unlock(&usb->usb_mutex));
}

int	put_usb(struct s_coder *s_arg, int direc)
{
	struct s_usb	*usb;

	usb = get_usb_by_direc(s_arg, direc);
	if (pthread_mutex_lock(&usb->usb_mutex) != 0)
		return (-1);
	usb->is_available = 1;
	usb->cdown_start = get_curr_time_ms();
	if (pthread_cond_broadcast(&usb->usb_rec_cond) != 0)
	{
		pthread_mutex_unlock(&usb->usb_mutex);
		return (-1);
	}
	if (pthread_mutex_unlock(&usb->usb_mutex) != 0)
		return (-1);
	return (0);
}
