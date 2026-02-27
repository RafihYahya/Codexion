/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 17:32:51 by yrafih            #+#    #+#             */
/*   Updated: 2026/02/27 03:29:45 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static void init_gstate(struct s_globalstate *gstate)
{
    gstate->cstates = NULL;
    gstate->mstate = NULL;
    gstate->pconfig = NULL;
    gstate->thd = NULL;
    gstate->states = NULL;
    gstate->scheduler = NULL;

    return ;
}

int main(int argc, char **argv)
{
    struct s_globalstate gstate;

    
    init_gstate(&gstate);
    gstate.pconfig = create_config(argc, argv);
    DEBUG("ParsedConfig: ",pconfig);
    if (!gstate.pconfig){
        ERROR("Couldn't Parse Argv");    
        return (-1);
    }
    // Init usb mutexes
    DEBUG("Starting Mutexes ");
    if (init_usb_mutexes_conds(gstate.pconfig->number_of_coders, &(gstate.states)) < 0)
    {
        ERROR("Failed Init of Mutexes");
        return (free(gstate.states), free(gstate.pconfig), -1); 
    }
    DEBUG("Finished Setting Mutexes");
    // Init scheduler
    DEBUG("Starting Scheduler");
    if (init_scheduler(&gstate) < 0)
    {
        ERROR("Can't Init Scheduler");
        return (free(gstate.states), free(gstate.pconfig), -1);   
    }
    DEBUG("Finished Setting Scheduler");
    // Setup print_mutex
    if (pthread_mutex_init(&(gstate.print_lock), NULL) < 0)
    {
        ERROR("Failure Setting up Print Mutex Lock");
        return (free(gstate.states), free(gstate.pconfig), -1);
    }
    // Init monitor thread
    DEBUG("Starting Monitor Thread");
    if (init_monitor_thread(&gstate) < 0)
    {
        ERROR("Failed Init of monitor thread");
        return (free(gstate.states), free(gstate.pconfig), -1);
    }
    DEBUG("Finished Setting Monitor Thread");
    // Init coder thread
    DEBUG("Starting Threads");
    if (init_coder_threads(&gstate, &(gstate.cstates), &(gstate.states), &(gstate.thd)) < 0)
    {
        ERROR("Failed Init of Mutexes");
        return (free(gstate.states), free(gstate.pconfig), free(gstate.mstate), -1); 
    }
    // Join monitor
    pthread_join(gstate.mstate->monitor, NULL);
    // Join threads ?
    DEBUG("End of Program");
    return (0);
}
