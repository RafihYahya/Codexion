/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	check_death(struct s_coder *s_arg)
{
	pthread_mutex_lock(&(s_arg->mstate->death_lock));
	if (s_arg->mstate->is_someone_dead == 1)
	{
		pthread_mutex_unlock(&(s_arg->mstate->death_lock));
		return (-1);
	}
	pthread_mutex_unlock(&(s_arg->mstate->death_lock));
	return (0);
}

int	safe_sleep(struct s_coder *s_arg, size_t ms)
{
	size_t	elapsed;

	elapsed = 0;
	while (elapsed < ms)
	{
		if (check_death(s_arg) != 0)
			return (-1);
		usleep(1000);
		elapsed++;
	}
	return (0);
}

void	*coder_thread(void *arg)
{
	struct s_coder	*s_arg;

	s_arg = (struct s_coder *)arg;
	while (1)
	{
		if (check_death(s_arg) != 0)
			return (NULL);
		if (s_arg->state == COMPILE)
			coder_thread_comp(s_arg);
		else if (s_arg->state == DEBUGGING && coder_thread_debug(s_arg) == 1)
			continue ;
		else if (s_arg->state == REFACTOR
			&& coder_thread_refactor(s_arg) != 0)
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
