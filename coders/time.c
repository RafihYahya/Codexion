/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alone <alone@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 03:53:58 by alone             #+#    #+#             */
/*   Updated: 2026/03/03 03:54:02 by alone            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

// time helper funcs
size_t get_curr_time_ms()
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
    {
        ERROR("Can't get the time");
        return (-1);
    }
    size_t ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return ms;
}

size_t check_time(struct s_CoderState *s_arg)
{
    size_t curr_time;

    curr_time = get_curr_time_ms();
    if (s_arg->state == COMPILE)
        return (curr_time - s_arg->arg->last_time_comp >= 
                (size_t)s_arg->gconfig->pconfig->time_to_compile);
    if (s_arg->state == DEBUGGING)
        return (curr_time - s_arg->arg->last_time_debug >= 
                (size_t)s_arg->gconfig->pconfig->time_to_debug);
    if (s_arg->state == REFACTOR)
        return (curr_time - s_arg->arg->last_time_refact >= 
                (size_t)s_arg->gconfig->pconfig->time_to_refactor);
    return (0);
}