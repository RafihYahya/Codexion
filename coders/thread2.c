#include "main.h"
#include <pthread.h>
#include <time.h>

static int get_abs_timeout_from_now_ms(size_t wait_ms, struct timespec *ts)
{
    struct timeval tv;
    size_t         usec_total;

    if (gettimeofday(&tv, NULL) != 0)
        return (-1);
    ts->tv_sec = tv.tv_sec + (time_t)(wait_ms / 1000);
    usec_total = (size_t)tv.tv_usec + (wait_ms % 1000) * 1000;
    ts->tv_sec += (time_t)(usec_total / 1000000);
    ts->tv_nsec = (long)((usec_total % 1000000) * 1000);
    return (0);
}


static int wait_for_my_turn(struct s_scheduler *sch, int my_id)
{
    struct s_fifo_queue *q;

    if (!sch || !sch->data)
        return (-1);
    if (pthread_mutex_lock(&sch->sched_lock) != 0)
        return (-1);
    q = (struct s_fifo_queue *)sch->data;

    while (!q->front || q->front->curr_id != my_id)
    {
        if (pthread_cond_wait(&sch->sched_id, &sch->sched_lock) != 0)
        {
            pthread_mutex_unlock(&sch->sched_lock);
            return (-1);
        }
    }
    if (pthread_mutex_unlock(&sch->sched_lock) != 0)
        return (-1);
    return (0);
}

static int take_usb(struct s_CoderState *s_arg, int direc)
{
    struct s_UsbDongleState *usb;
    size_t                  now_ms;
    size_t                  ready_ms;
    size_t                  wait_ms;
    struct timespec         ts;

    usb = (direc == 0) ? s_arg->l_usb : s_arg->r_usb;
    if (pthread_mutex_lock(&usb->usb_mutex) != 0)
        return (-1);
    while (1)
    {
        now_ms = get_curr_time_ms();
        if (usb->is_available == 1)
        {
            ready_ms = usb->cdown_start + s_arg->gconfig->pconfig->dongle_cooldown;
            if (usb->cdown_start == 0 || now_ms >= ready_ms)
            {
                usb->is_available = 0;
                break ;
            }
            wait_ms = ready_ms - now_ms;
            if (get_abs_timeout_from_now_ms(wait_ms, &ts) != 0)
            {
                pthread_mutex_unlock(&usb->usb_mutex);
                return (-1);
            }
            if (pthread_cond_timedwait(&usb->usb_rec_cond, &usb->usb_mutex, &ts) != 0)
            {
                now_ms = get_curr_time_ms();
                if (now_ms < ready_ms)
                {
                    pthread_mutex_unlock(&usb->usb_mutex);
                    return (-1);
                }
            }
        }
        else if (pthread_cond_wait(&usb->usb_rec_cond, &usb->usb_mutex) != 0)
        {
            pthread_mutex_unlock(&usb->usb_mutex);
            return (-1);
        }
    }
    if (pthread_mutex_unlock(&usb->usb_mutex) != 0)
        return (-1);
    return (0);
}

static int put_usb(struct s_CoderState *s_arg, int direc)
{
    struct s_UsbDongleState *usb;

    usb = (direc == 0) ? s_arg->l_usb : s_arg->r_usb;
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

void coder_thread_comp(struct s_CoderState *s_arg)
{
    struct s_scheduler *sch;

    sch = (struct s_scheduler *)s_arg->gconfig->scheduler;
    if (!s_arg->is_queued)
    {
        if (sch->add_task(sch, s_arg->id, 0) == 0)
            s_arg->is_queued = 1;
        else
            return ;
    }
    if (wait_for_my_turn(s_arg->gconfig->scheduler, s_arg->id) != 0)
        return ;

    if (take_usb(s_arg, 0) != 0)
        return ;
    if (take_usb(s_arg, 1) != 0)
    {
        put_usb(s_arg, 0);
        return ;
    }

    usleep(s_arg->gconfig->pconfig->time_to_compile * 1000);
    s_arg->state = DEBUGGING;
    s_arg->arg->last_time_debug = get_curr_time_ms();

    if (put_usb(s_arg, 1) != 0)
        return ;
    if (put_usb(s_arg, 0) != 0)
        return ;

    sch->task_finished(sch, s_arg->id);
    s_arg->is_queued = 0;
    if (pthread_cond_broadcast(&sch->sched_id) != 0)
        return ;
}

int  coder_thread_refactor(struct s_CoderState *s_arg)
{
    if (check_time(s_arg))
            {
                s_arg->arg->compiled_count++;
                if (s_arg->arg->compiled_count >= 
                    s_arg->gconfig->pconfig->number_of_compiles_required)
                {
                    DEBUG("Finished Required Compiles Count");
                    return (-1);
                }
                s_arg->state = COMPILE;
                s_arg->arg->last_time_comp = get_curr_time_ms();
            }
            usleep(s_arg->gconfig->pconfig->time_to_refactor);
    return (0);
}