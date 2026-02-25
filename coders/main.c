/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 17:32:51 by yrafih            #+#    #+#             */
/*   Updated: 2026/02/25 17:17:31 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

struct s_ArgvParsedConfig *create_config(int argc, char **argv)
{
    struct s_ArgvParsedConfig *cfg;

    cfg = malloc(sizeof(*cfg));
    if (!cfg)
        return (NULL);

    #ifdef TESTING
    *parsed_argv = DEFAULT_CONFIG;
    #else
    if (argv_parser_validator(argc, argv, cfg) < 0)
        {
            ERROR("Parsing Error");
            free(cfg);
            return NULL;
        }
    #endif
    return (cfg);
}

int main(int argc, char **argv)
{
    struct s_globalstate gstate;

    gstate.cstates = NULL;
    gstate.mstate = NULL;
    gstate.pconfig = NULL;
    gstate.thd = NULL;
    gstate.states = NULL;

    gstate.pconfig = create_config(argc, argv);
    DEBUG("ParsedConfig: ",pconfig);
    if (!gstate.pconfig)
        return (-1);
    if (init_usb_mutexes(gstate.pconfig->number_of_coders, &(gstate.states)) < 0)
    {
        ERROR("Failed Init of Mutexes");
        return (free(gstate.states), free(gstate.pconfig), -1); 
    }
    // init scheduler
    if (init_coder_threads(&gstate, &(gstate.cstates), &(gstate.states), &(gstate.thd)) < 0)
    {
        ERROR("Failed Init of Mutexes");
        return (free(gstate.states), free(gstate.pconfig), -1); 
    }
    // init monitor
    // join monitor
    // join threads
    // finish 
    return (0);
}
