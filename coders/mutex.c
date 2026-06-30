/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	init_usb_mutexes_conds(int num_usb, struct s_usb **usb_mutexes)
{
	int	i;

	i = 0;
	if (!usb_mutexes)
		return (put_error("Error: usb array must not be null"), -2);
	*usb_mutexes = malloc(sizeof(struct s_usb) * num_usb);
	if (!*usb_mutexes)
		return (put_error("Error: malloc failed"), -1);
	while (i < num_usb)
	{
		if (pthread_mutex_init(&(*usb_mutexes)[i].usb_mutex, NULL) != 0
			|| pthread_cond_init(&(*usb_mutexes)[i].usb_rec_cond, NULL) != 0)
		{
			put_error("Error: could not init mutexes");
			return (free(*usb_mutexes), -1);
		}
		(*usb_mutexes)[i].cdown_start = 0;
		(*usb_mutexes)[i].is_available = 1;
		(*usb_mutexes)[i].waitq = NULL;
		i++;
	}
	return (0);
}
