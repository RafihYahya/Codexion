/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 03:54:23 by alone             #+#    #+#             */
/*   Updated: 2026/04/01 16:06:04 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

struct s_ArgvParsedConfig DEFAULT_CONFIG = {
        .number_of_coders = 1,
        .time_to_burnout = 100,
        .time_to_compile = 20,
        .time_to_debug = 30,
        .time_to_refactor = 10,
        .number_of_compiles_required = 15,
        .dongle_cooldown = 10,
        .scheduler = "fifo"
        };

struct s_ArgvParsedConfig *create_config(int argc, char **argv)
{
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

static int ft_isdigit_str(const char *s)
{
    if (!s || !*s)
        return (0);
    while (*s)
    {
        if (*s < '0' || *s > '9')
            return (0);
        s++;
    }
    return (1);
}

static int ft_validate_args(int argc, char **argv)
{
    int i;

    if (argc != 9)
    {
        ERROR("Usage: %s number_of_coders time_to_burnout time_to_compile"
              " time_to_debug time_to_refactor number_of_compiles_required"
              " dongle_cooldown scheduler(fifo|edf)", argv[0]);
        return (-1);
    }
    i = 1;
    while (i <= 7)
    {
        if (!ft_isdigit_str(argv[i]))
            return (ERROR("Argument %d must be a positive integer: '%s'",
                    i, argv[i]), -1);
        i++;
    }
    if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
        return (ERROR("scheduler must be 'fifo' or 'edf', got: '%s'",
                argv[8]), -1);
    return (0);
}

static int ft_parse_nums(char **argv, int out[7])
{
    int i;

    i = 0;
    while (i < 7)
    {
        out[i] = atoi(argv[i + 1]);
        if (out[i] <= 0)
            return (ERROR("Argument %d must be strictly positive", i + 1), -1);
        i++;
    }
    return (0);
}

static void ft_fill_config(struct s_ArgvParsedConfig *s, int v[7], char *sched)
{
    s->number_of_coders            = (unsigned int)v[0];
    s->time_to_burnout             = (size_t)v[1];
    s->time_to_compile             = (size_t)v[2];
    s->time_to_debug               = (size_t)v[3];
    s->time_to_refactor            = (size_t)v[4];
    s->number_of_compiles_required = (unsigned int)v[5];
    s->dongle_cooldown             = (size_t)v[6];
    strncpy(s->scheduler, sched, sizeof(s->scheduler) - 1);
    s->scheduler[sizeof(s->scheduler) - 1] = '\0';
}

int argv_parser_validator(int argc, char **argv, struct s_ArgvParsedConfig *s)
{
    int v[7];

    if (ft_validate_args(argc, argv) < 0)
        return (-1);
    if (ft_parse_nums(argv, v) < 0)
        return (-1);
    ft_fill_config(s, v, argv[8]);
    return (0);
}
