/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 17:32:51 by yrafih            #+#    #+#             */
/*   Updated: 2026/02/28 02:39:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

#define DUMP_GSTATE(g) DEBUG("\ngstate State:\n" \
    "  pconfig->number_of_coders: %d\n" \
    "  pconfig->time_to_burnout: %d\n" \
    "  pconfig->time_to_compile: %d\n" \
    "  pconfig->time_to_debug: %d\n" \
    "  pconfig->time_to_refactor: %d\n" \
    "  pconfig->number_of_compiles_required: %d\n" \
    "  pconfig->dongle_cooldown: %d\n" \
    "  pconfig->scheduler: %s\n" \
    "  mstate->is_someone_dead: %d\n" \
    "  mstate ptr: %p\n" \
    "  cstates ptr: %p\n" \
    "  states ptr: %p\n" \
    "  thd ptr: %p\n" \
    "  scheduler ptr: %p\n", \
    (g).pconfig->number_of_coders, \
    (g).pconfig->time_to_burnout, \
    (g).pconfig->time_to_compile, \
    (g).pconfig->time_to_debug, \
    (g).pconfig->time_to_refactor, \
    (g).pconfig->number_of_compiles_required, \
    (g).pconfig->dongle_cooldown, \
    (g).pconfig->scheduler, \
    (g).mstate->is_someone_dead, \
    (void *)(g).mstate, \
    (void *)(g).cstates, \
    (void *)(g).states, \
    (void *)(g).thd, \
    (void *)(g).scheduler)

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

void mem_cleanup(struct s_globalstate *gstate)
{
    int i = 0;

    pthread_mutex_destroy(&gstate->print_lock);
    if (gstate->mstate)
    {
        pthread_mutex_destroy(&gstate->mstate->death_lock);
        free(gstate->mstate);
    }
    if (gstate->cstates)
    {
        while (i < gstate->pconfig->number_of_coders)
            free(gstate->cstates[i++].arg);
        free(gstate->cstates);
    }
    free(gstate->thd);
    free(gstate->states);
    free(gstate->pconfig);
    free(gstate->scheduler);
}

int main(int argc, char **argv)
{
    struct s_globalstate gstate;

    
    init_gstate(&gstate);
    gstate.pconfig = create_config(argc, argv);
    DEBUG("Finished Parsing Configs");
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
    // Init Monitor thread
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
    DEBUG("Waiting Here Until Monitor Join");
    pthread_join(gstate.mstate->monitor, NULL);
    // Join threads
    int k = 0;
    while (k < gstate.pconfig->number_of_coders)
    {
        pthread_join(gstate.thd[k], NULL);
        k++;
    }
    // Finished
    DUMP_GSTATE(gstate);
    DEBUG("End of Codexion Program");
    mem_cleanup(&gstate);
    return (0);
}
