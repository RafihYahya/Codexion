/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   policy_conc.c                                      :+:      :+:    :+:   */
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
** Per-dongle arbitration. The granted coder is always the queue head, so only
** the insertion order is policy-specific. FIFO appends (arrival order); EDF
** inserts by earliest deadline, breaking ties by higher coder id.
*/
static int	fifo_enqueue(struct s_usb *d, int id, long deadline)
{
	struct s_req	*node;
	struct s_req	*cur;

	node = malloc(sizeof(struct s_req));
	if (!node)
		return (-1);
	node->id = id;
	node->deadline = deadline;
	node->next = NULL;
	if (!d->waitq)
		return (d->waitq = node, 0);
	cur = d->waitq;
	while (cur->next)
		cur = cur->next;
	cur->next = node;
	return (0);
}

static int	edf_enqueue(struct s_usb *d, int id, long deadline)
{
	struct s_req	*node;
	struct s_req	*cur;

	node = malloc(sizeof(struct s_req));
	if (!node)
		return (-1);
	node->id = id;
	node->deadline = deadline;
	node->next = NULL;
	cur = d->waitq;
	if (!cur || deadline < cur->deadline
		|| (deadline == cur->deadline && id > cur->id))
		return (node->next = cur, d->waitq = node, 0);
	while (cur->next && (cur->next->deadline < deadline
			|| (cur->next->deadline == deadline && cur->next->id > id)))
		cur = cur->next;
	node->next = cur->next;
	cur->next = node;
	return (0);
}

int	init_scheduler(struct s_globalstate *gstate)
{
	if (!gstate)
		return (-1);
	gstate->policy = malloc(sizeof(struct s_policy));
	if (!gstate->policy)
		return (put_error("Error: malloc policy failed"), -1);
	if (strcmp(gstate->pconfig->scheduler, "edf") == 0)
		gstate->policy->enqueue = edf_enqueue;
	else
		gstate->policy->enqueue = fifo_enqueue;
	return (0);
}

void	cleanup_sched(struct s_globalstate *gstate)
{
	if (gstate->policy)
		free(gstate->policy);
}

#endif
