/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_init.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alone <alone@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 04:30:58 by alone             #+#    #+#             */
/*   Updated: 2026/03/03 04:31:01 by alone            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"


static void init_one_coder_state(struct s_globalstate *gstate,
    size_t i, struct s_CoderArg *carg)
{
    carg->last_time_comp = get_curr_time_ms();
    carg->last_time_debug = 0;
    carg->last_time_refact = 0;
    carg->compiled_count = 0;
    gstate->cstates[i].id = i;
    gstate->cstates[i].is_queued = 0;
    gstate->cstates[i].gconfig = gstate;
    gstate->cstates[i].state = COMPILE;
    gstate->cstates[i].mstate = gstate->mstate;
    gstate->cstates[i].l_usb = &gstate->states[i % gstate->pconfig->number_of_coders];
    gstate->cstates[i].r_usb = &gstate->states[(i + 1)
        % gstate->pconfig->number_of_coders];
    gstate->cstates[i].arg = carg;
}

static int  init_coder_threads_helper(struct s_globalstate *gstate)
{   
    size_t                i;
    size_t                j;
    struct s_CoderArg *carg;

    i = 0;
    carg = NULL;
    while (i < (size_t)gstate->pconfig->number_of_coders)
    {
        carg = malloc(sizeof(struct s_CoderArg));
        if (!carg)
        {
            ERROR("Failed Allocation");
            j = 0;
            while (j < i)
                free(gstate->cstates[j++].arg);
            return (free(gstate->thd), free(gstate->cstates), -1);
        }
        init_one_coder_state(gstate, i, carg);
        if (pthread_create(&gstate->thd[i], NULL, coder_thread, &gstate->cstates[i]) != 0)
        {
            ERROR("Failed creation of threads");
            return (free(gstate->thd), free(gstate->cstates), -1);
        }
        i++;
    }
    return (0);
}

int init_coder_threads(struct s_globalstate *gstate)
{

    DEBUG("Creation of Coder Threads along with their corresponding states");
    if (!gstate || !gstate->states || !gstate->pconfig)
    {
        ERROR("Arguments Invalid/Null");
        return (-1);
    }
    DEBUG("Setting up states and thread allocations");
    gstate->cstates = malloc(sizeof(struct s_CoderState)
            * gstate->pconfig->number_of_coders);
    gstate->thd = malloc(sizeof(pthread_t) * gstate->pconfig->number_of_coders);
    if (!gstate->thd || !gstate->cstates)
    {
        ERROR("Failed Allocation");
        return (-1);
    }
    if (init_coder_threads_helper(gstate) != 0)
        return (-1);
    DEBUG("End of Thread Init function");
    return (0);
}