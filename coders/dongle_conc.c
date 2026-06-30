/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_conc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/30 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

#ifdef USE_CONCURRENT

static int	req_peek(struct s_usb *usb)
{
	if (!usb->waitq)
		return (-1);
	return (usb->waitq->id);
}

static void	req_remove(struct s_usb *usb, int id)
{
	struct s_req	*cur;
	struct s_req	*prev;

	cur = usb->waitq;
	prev = NULL;
	while (cur)
	{
		if (cur->id == id)
		{
			if (prev)
				prev->next = cur->next;
			else
				usb->waitq = cur->next;
			free(cur);
			return ;
		}
		prev = cur;
		cur = cur->next;
	}
}

static int	can_take(struct s_usb *usb, struct s_coder *s_arg, size_t now)
{
	size_t	cd;

	if (req_peek(usb) != (int)s_arg->id || usb->is_available != 1)
		return (0);
	cd = s_arg->gconfig->pconfig->dongle_cooldown;
	if (usb->cdown_start != 0 && now < usb->cdown_start + cd)
		return (0);
	return (1);
}

int	take_dongle(struct s_coder *s_arg, struct s_usb *usb)
{
	struct timespec	ts;
	long			dl;

	dl = (long)s_arg->arg->last_time_comp;
	dl += (long)s_arg->gconfig->pconfig->time_to_burnout;
	pthread_mutex_lock(&usb->usb_mutex);
	if (s_arg->gconfig->policy->enqueue(usb, (int)s_arg->id, dl) != 0)
		return (pthread_mutex_unlock(&usb->usb_mutex), -1);
	while (!can_take(usb, s_arg, get_curr_time_ms()))
	{
		if (check_death(s_arg) != 0)
		{
			req_remove(usb, (int)s_arg->id);
			return (pthread_mutex_unlock(&usb->usb_mutex), -1);
		}
		if (get_abs_timeout_from_now_ms(1, &ts) == 0)
			pthread_cond_timedwait(&usb->usb_rec_cond, &usb->usb_mutex, &ts);
	}
	req_remove(usb, (int)s_arg->id);
	usb->is_available = 0;
	log_state((struct s_globalstate *)s_arg->gconfig,
		get_curr_time_ms() - s_arg->gconfig->start_time_ms,
		s_arg->id + 1, "has taken a dongle");
	return (pthread_mutex_unlock(&usb->usb_mutex), 0);
}

int	put_dongle(struct s_coder *s_arg, struct s_usb *usb)
{
	(void)s_arg;
	if (pthread_mutex_lock(&usb->usb_mutex) != 0)
		return (-1);
	usb->is_available = 1;
	usb->cdown_start = get_curr_time_ms();
	pthread_cond_broadcast(&usb->usb_rec_cond);
	return (pthread_mutex_unlock(&usb->usb_mutex), 0);
}

#endif
