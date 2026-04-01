/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 03:54:23 by alone             #+#    #+#             */
/*   Updated: 2026/04/01 15:26:21 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"


struct s_ArgvParsedConfig DEFAULT_CONFIG = {
        .number_of_coders = 4,
        .time_to_burnout = 800,
        .time_to_compile = 2000,
        .time_to_debug = 200,
        .time_to_refactor = 200,
        .number_of_compiles_required = 15,
        .dongle_cooldown = 500,
        .scheduler = "fifo"
        };

struct s_ArgvParsedConfig *create_config(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    struct s_ArgvParsedConfig *cfg;

    cfg = malloc(sizeof(*cfg));
    if (!cfg)
        return (NULL);

    #ifdef SKIP_PARSE
    *cfg = DEFAULT_CONFIG;
    #else
    if (argv_parser_validator(argc, argv, cfg) < 0)
        {
            ERROR("Parsing Error");
            return (free(cfg), NULL);
        }
    #endif
    return (cfg);
}

int argv_parser_validator(int argc, char **argv, struct s_ArgvParsedConfig *s)
{
    (void)argc;
    (void)argv;
    (void)s;
	return 0;
}


