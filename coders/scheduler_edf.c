/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_edf.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

/*
** EDF order: earliest deadline first. Tie-break: higher coder id first,
** which guarantees a fully deterministic policy on equal deadlines.
*/
static void	edf_insert(struct s_edf_node **head, struct s_edf_node *node)
{
	struct s_edf_node	*cur;

	cur = *head;
	if (!cur || node->deadline < cur->deadline
		|| (node->deadline == cur->deadline && node->curr_id > cur->curr_id))
	{
		node->next = cur;
		*head = node;
		return ;
	}
	while (cur->next && (cur->next->deadline < node->deadline
			|| (cur->next->deadline == node->deadline
				&& cur->next->curr_id > node->curr_id)))
		cur = cur->next;
	node->next = cur->next;
	cur->next = node;
}

static int	edf_add_task(struct s_scheduler *sch, int id, long deadline)
{
	struct s_edf_node	*node;

	if (!sch)
		return (-1);
	node = malloc(sizeof(struct s_edf_node));
	if (!node)
		return (-1);
	node->curr_id = id;
	node->deadline = deadline;
	node->next = NULL;
	pthread_mutex_lock(&sch->sched_lock);
	edf_insert((struct s_edf_node **)&sch->data, node);
	pthread_mutex_unlock(&sch->sched_lock);
	return (0);
}

static void	edf_task_finished(struct s_scheduler *sch, int id)
{
	struct s_edf_node	**head;
	struct s_edf_node	*prev;
	struct s_edf_node	*cur;

	if (!sch || !sch->data)
		return ;
	pthread_mutex_lock(&sch->sched_lock);
	head = (struct s_edf_node **)&sch->data;
	cur = *head;
	prev = NULL;
	while (cur)
	{
		if (cur->curr_id == id)
		{
			if (prev)
				prev->next = cur->next;
			else
				*head = cur->next;
			free(cur);
			break ;
		}
		prev = cur;
		cur = cur->next;
	}
	pthread_mutex_unlock(&sch->sched_lock);
}

static int	edf_pick_next(struct s_scheduler *sch, int *out_id, long *out_dl)
{
	struct s_edf_node	*head;

	if (!sch || !sch->data)
		return (-1);
	head = (struct s_edf_node *)sch->data;
	*out_id = head->curr_id;
	*out_dl = head->deadline;
	return (0);
}

int	init_edf_scheduler(struct s_globalstate *gstate)
{
	gstate->scheduler->data = NULL;
	gstate->scheduler->add_task = edf_add_task;
	gstate->scheduler->task_finished = edf_task_finished;
	gstate->scheduler->remove_task = NULL;
	gstate->scheduler->pick_next = edf_pick_next;
	return (0);
}
