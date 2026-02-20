/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 17:32:51 by yrafih            #+#    #+#             */
/*   Updated: 2026/02/20 17:52:28 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"



int main(int argc, char **argv)
{
    int     current_error;
    struct s_ArgvParsedConfig *parsed_argv;

    parsed_argv = NULL;
    current_error = argv_parser_validator(argc, argv, parsed_argv);
    if (current_error < 0)
    {
        ERROR("Failed Parsing");
        return (-1);
    }
    
}