/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 00:00:00 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	ft_isdigit_str(const char *s)
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

static int	ft_validate_args(int argc, char **argv)
{
	int	i;

	if (argc != 9)
		return (put_error("usage: codexion n_coders burnout compile debug "
				"refactor compiles cooldown scheduler"), -1);
	i = 1;
	while (i <= 7)
	{
		if (!ft_isdigit_str(argv[i]))
			return (put_error("Error: arguments must be positive integers"),
				-1);
		i++;
	}
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
		return (put_error("Error: scheduler must be fifo or edf"), -1);
	return (0);
}

static int	ft_parse_nums(char **argv, int *out)
{
	int	i;

	i = 0;
	while (i < 7)
	{
		out[i] = atoi(argv[i + 1]);
		if (out[i] <= 0)
			return (put_error("Error: numeric arguments must be positive"),
				-1);
		i++;
	}
	return (0);
}

int	argv_parser_validator(int argc, char **argv, struct s_config *s)
{
	int		v[7];
	size_t	i;

	if (ft_validate_args(argc, argv) < 0)
		return (-1);
	if (ft_parse_nums(argv, v) < 0)
		return (-1);
	s->number_of_coders = (unsigned int)v[0];
	s->time_to_burnout = (size_t)v[1];
	s->time_to_compile = (size_t)v[2];
	s->time_to_debug = (size_t)v[3];
	s->time_to_refactor = (size_t)v[4];
	s->number_of_compiles_required = (unsigned int)v[5];
	s->dongle_cooldown = (size_t)v[6];
	i = 0;
	while (argv[8][i] && i + 1 < sizeof(s->scheduler))
	{
		s->scheduler[i] = argv[8][i];
		i++;
	}
	s->scheduler[i] = '\0';
	return (0);
}

struct s_config	*create_config(int argc, char **argv)
{
	struct s_config	*cfg;

	cfg = malloc(sizeof(*cfg));
	if (!cfg)
		return (NULL);
	if (argv_parser_validator(argc, argv, cfg) < 0)
		return (free(cfg), NULL);
	return (cfg);
}
