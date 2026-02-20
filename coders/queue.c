/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 17:33:11 by yrafih            #+#    #+#             */
/*   Updated: 2026/02/20 22:38:38 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "queue.h"
#include <unistd.h>

int init_queue(struct s_Queue **q, void *value, struct s_Queue *next)
{
	struct s_Queue *tmp;

	if (!q || !*q)
		return (-1);
	tmp = *q;
	tmp->value = value;
	tmp->next = next;
	return (0);
}

int append_to_queue(struct s_Queue **q, void *value)
{
	
}
