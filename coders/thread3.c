/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alone <alone@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 04:58:13 by alone             #+#    #+#             */
/*   Updated: 2026/03/03 04:58:14 by alone            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int mark_usb_taken(struct s_CoderState *s_arg,
    struct s_UsbDongleState *usb)
{
    usb->is_available = 0;
    SAFE_PRINT((struct s_globalstate *)s_arg->gconfig,
        "%zu %u has taken a dongle\n",
        get_curr_time_ms() - s_arg->gconfig->start_time_ms,
        s_arg->id + 1);
    return (1);
}

static int wait_usb_cooldown(struct s_CoderState *s_arg,
    struct s_UsbDongleState *usb, size_t now_ms)
{
    size_t          ready_ms;
    size_t          wait_ms;
    struct timespec ts;

    ready_ms = usb->cdown_start + s_arg->gconfig->pconfig->dongle_cooldown;
    if (usb->cdown_start == 0 || now_ms >= ready_ms)
        return (mark_usb_taken(s_arg, usb));
    wait_ms = ready_ms - now_ms;
    if (get_abs_timeout_from_now_ms(wait_ms, &ts) != 0)
        return (-1);
    if (pthread_cond_timedwait(&usb->usb_rec_cond, &usb->usb_mutex, &ts) != 0)
    {
        now_ms = get_curr_time_ms();
        if (now_ms < ready_ms)
            return (-1);
    }
    return (0);
}

static struct s_UsbDongleState *get_usb_by_direc(struct s_CoderState *s_arg,
    int direc)
{
    if (direc == 0)
        return (s_arg->l_usb);
    return (s_arg->r_usb);
}

int take_usb(struct s_CoderState *s_arg, int direc)
{
    struct s_UsbDongleState *usb;
    size_t                  now_ms;
    int                     status;

    usb = get_usb_by_direc(s_arg, direc);
    if (pthread_mutex_lock(&usb->usb_mutex) != 0)
        return (-1);
    while (1)
    {
        now_ms = get_curr_time_ms();
        if (usb->is_available == 1)
        {
            status = wait_usb_cooldown(s_arg, usb, now_ms);
            if (status < 0)
                return (pthread_mutex_unlock(&usb->usb_mutex), -1);
            if (status > 0)
                break ;
        }
        else if (pthread_cond_wait(&usb->usb_rec_cond, &usb->usb_mutex) != 0)
            return (pthread_mutex_unlock(&usb->usb_mutex), -1);
    }
    if (pthread_mutex_unlock(&usb->usb_mutex) != 0)
        return (-1);
    return (0);
}

int put_usb(struct s_CoderState *s_arg, int direc)
{
    struct s_UsbDongleState *usb;

    usb = get_usb_by_direc(s_arg, direc);
    if (pthread_mutex_lock(&usb->usb_mutex) != 0)
        return (-1);
    usb->is_available = 1;
    usb->cdown_start = get_curr_time_ms();
    if (pthread_cond_broadcast(&usb->usb_rec_cond) != 0)
    {
        pthread_mutex_unlock(&usb->usb_mutex);
        return (-1);
    }
    if (pthread_mutex_unlock(&usb->usb_mutex) != 0)
        return (-1);
    return (0);
}