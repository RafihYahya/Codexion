/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 17:32:51 by yrafih            #+#    #+#             */
/*   Updated: 2026/02/22 22:15:15 by yrafih           ###   ########.fr       */
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
    struct s_ArgvParsedConfig *pconfig;
    struct s_UsbDongleState *states;

    pconfig = create_config(argc, argv);
    DEBUG("ParsedConfig: ",pconfig);
    if (!pconfig)
        return (-1);
    if (init_usb_mutexes(pconfig->number_of_coders, states) < 0)
    {
        ERROR("Failed Init of Mutexes");
        return (free(states), free(pconfig), -1); 
    }
    // create usb dongles and set mutexes
    // create coders(threads) and setup each one state
    // init scheduler
    // output
}
