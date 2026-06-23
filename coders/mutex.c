/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alone <alone@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 03:54:25 by alone             #+#    #+#             */
/*   Updated: 2026/03/03 04:54:49 by alone            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int init_usb_mutexes_conds(
    int num_usb, struct s_UsbDongleState **usb_mutexes)
{
    int i;

    i = 0;
    if (!usb_mutexes)
    {
        ERROR("Must Not be Null ");
        return (-2);
    }
    *usb_mutexes = malloc(sizeof(struct s_UsbDongleState) * num_usb);
    if (!*usb_mutexes)
    {
        ERROR("Malloc Has Failed, Just like most of your dreams");
        return (-1);
    }
    DEBUG("Initiating Mutex");
    while (i < num_usb)
    {
        if (
            (pthread_mutex_init(&(*usb_mutexes)[i].usb_mutex, NULL) != 0)
            || 
            (pthread_cond_init(&(*usb_mutexes)[i].usb_rec_cond, NULL) != 0)
            )
        {
            ERROR("Couldn't Fully Init Mutexes");
            return (free(*usb_mutexes), -1);
        }
        (*usb_mutexes)[i].cdown_start = 0;
        (*usb_mutexes)[i].is_available = 1;
        i++;
    }
    DEBUG("Finished Setting up usb_conds_mutexes");
    return (0);
}