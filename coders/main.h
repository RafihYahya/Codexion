/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 03:54:31 by yrafih            #+#    #+#             */
/*   Updated: 2026/06/29 00:00:00 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

/* Comment the next line to build the original (serialized) scheduler model. */
# define USE_CONCURRENT

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>

enum e_state
{
	COMPILE,
	DEBUGGING,
	REFACTOR
};

struct s_config
{
	unsigned int	number_of_coders;
	unsigned int	number_of_compiles_required;
	size_t			time_to_burnout;
	size_t			time_to_compile;
	size_t			time_to_debug;
	size_t			time_to_refactor;
	size_t			dongle_cooldown;
	char			scheduler[8];
};

struct s_req
{
	int				id;
	long			deadline;
	struct s_req	*next;
};

struct s_usb
{
	pthread_mutex_t	usb_mutex;
	pthread_cond_t	usb_rec_cond;
	int				is_available;
	size_t			cdown_start;
	struct s_req	*waitq;
};

struct s_coder_arg
{
	size_t			last_time_comp;
	size_t			last_time_refact;
	size_t			last_time_debug;
	unsigned int	compiled_count;
};

struct s_monitorstate
{
	int				is_someone_dead;
	unsigned int	finished_coders;
	pthread_t		monitor;
	pthread_mutex_t	death_lock;
};

struct s_scheduler
{
	int				(*add_task)(struct s_scheduler *s, int id, long dl);
	int				(*remove_task)(struct s_scheduler *s, int id);
	void			(*task_finished)(struct s_scheduler *s, int id);
	int				(*pick_next)(struct s_scheduler *s, int *id, long *dl);
	pthread_mutex_t	sched_lock;
	pthread_cond_t	sched_id;
	void			*data;
};

struct s_policy
{
	int	(*enqueue)(struct s_usb *d, int id, long deadline);
};

struct s_globalstate
{
	const struct s_config	*pconfig;
	struct s_usb			*states;
	struct s_coder			*cstates;
	struct s_monitorstate	*mstate;
	struct s_scheduler		*scheduler;
	struct s_policy			*policy;
	size_t					start_time_ms;
	pthread_mutex_t			print_lock;
	pthread_t				*thd;
};

struct s_coder
{
	unsigned int				id;
	unsigned int				is_queued;
	enum e_state				state;
	struct s_usb				*l_usb;
	struct s_usb				*r_usb;
	struct s_coder_arg			*arg;
	struct s_monitorstate		*mstate;
	const struct s_globalstate	*gconfig;
};

struct s_node
{
	int				curr_id;
	struct s_node	*next;
};

struct s_fifo_queue
{
	struct s_node	*front;
	struct s_node	*back;
	size_t			count;
};

struct s_edf_node
{
	int					curr_id;
	long				deadline;
	struct s_edf_node	*next;
};

/*
** Old-model random scheduler queue. Keep "head" first: it is layout-compatible
** with s_fifo_queue's "front", so free_fifo_data (scheduler.c) also frees this.
*/
struct s_rand_queue
{
	struct s_node	*head;
	size_t			count;
	int				pick_id;
	unsigned int	seed;
};

size_t			get_curr_time_ms(void);
size_t			check_time(struct s_coder *s_arg);
size_t			get_abs_timeout_from_now_ms(size_t wait_ms,
					struct timespec *ts);
void			put_error(const char *msg);
void			log_state(struct s_globalstate *g, size_t ts,
					unsigned int id, const char *msg);
void			coder_thread_comp(struct s_coder *s_arg);
void			mem_cleanup(struct s_globalstate *gstate);
void			cleanup_sched(struct s_globalstate *gstate);
void			*coder_thread(void *arg);
struct s_config	*create_config(int argc, char **argv);
int				init_monitor_state(struct s_globalstate *gstate);
int				start_monitor(struct s_globalstate *gstate);
int				init_usb_mutexes_conds(int num_usb,
					struct s_usb **usb_mutexes);
int				init_scheduler(struct s_globalstate *gstate);
int				argv_parser_validator(int argc, char **argv,
					struct s_config *s);
int				init_fifo_scheduler(struct s_globalstate *gstate);
int				init_edf_scheduler(struct s_globalstate *gstate);
int				init_random_scheduler(struct s_globalstate *gstate);
int				coder_thread_refactor(struct s_coder *s_arg);
int				init_coder_threads(struct s_globalstate *gstate);
int				coder_thread_debug(struct s_coder *s_arg);
int				safe_sleep(struct s_coder *s_arg, size_t ms);
int				take_usb(struct s_coder *s_arg, int direc);
int				put_usb(struct s_coder *s_arg, int direc);
int				take_dongle(struct s_coder *s_arg, struct s_usb *usb);
int				put_dongle(struct s_coder *s_arg, struct s_usb *usb);
int				check_death(struct s_coder *s_arg);

#endif
