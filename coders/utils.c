/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

void	put_error(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
}

void	log_state(struct s_globalstate *g, size_t ts, unsigned int id,
		const char *msg)
{
	pthread_mutex_lock(&g->print_lock);
	printf("%zu %u %s\n", ts, id, msg);
	pthread_mutex_unlock(&g->print_lock);
}
