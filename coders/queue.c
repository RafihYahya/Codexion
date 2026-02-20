/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 17:33:11 by yrafih            #+#    #+#             */
/*   Updated: 2026/02/20 23:06:29 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "main.h"

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
	struct s_Queue *tmp;

	if (!q || !*q)
		return (-1);
	tmp = *q;
	while (tmp->next) {
		tmp = tmp->next;
	}
	tmp = malloc(sizeof(struct s_Queue));
	if (!tmp) {
		ERROR("Allocation Failed");
		return (-1);
	}
	tmp->value = value;
	tmp->next = NULL;
	return (0);
}

int pop_from_queue(struct s_Queue **q)
{
	struct s_Queue *tmp;
	struct s_Queue *front;

	if (!q || !*q)
		return (-1);
	front = *q;
	if(!front->next)
	{
		free(front);
		*q = NULL;
		return(0);
	}
	tmp = front->value;
	*q = tmp;
	free(front);
	return (0);
}
