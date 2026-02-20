/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 17:32:51 by yrafih            #+#    #+#             */
/*   Updated: 2026/02/20 20:49:05 by yrafih           ###   ########.fr       */
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

    pconfig = create_config(argc, argv);
    if (!pconfig)
        return (-1);
}
