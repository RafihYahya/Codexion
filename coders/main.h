/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrafih <yrafih@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 03:54:31 by alone             #+#    #+#             */
/*   Updated: 2026/04/01 15:29:21 by yrafih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
    #define MAIN_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>


struct s_globalstate {
    const struct s_ArgvParsedConfig   *pconfig;
    struct s_UsbDongleState     *states;
    struct s_CoderState         *cstates;
    struct s_monitorstate       *mstate;
    struct s_scheduler          *scheduler;
    size_t                      start_time_ms;
    pthread_mutex_t              print_lock;
    pthread_t                   *thd;
};

struct s_node {
    int curr_id;
    struct s_node *next;
};

struct s_fifo_queue
{
    struct s_node *front;
    struct s_node *back;
    size_t count;
};

struct s_monitorstate {
    int is_someone_dead;
    unsigned int finished_coders;
    pthread_t monitor;
    pthread_mutex_t death_lock;
};

struct s_scheduler {
    int (*add_task)(struct s_scheduler*, int id, long deadline);
    int (*remove_task)(struct s_scheduler*, int id);
    void (*task_finished)(struct s_scheduler *, int id);
    int (*pick_next)(struct s_scheduler*, int *out_id, long *out_deadline);

    pthread_mutex_t sched_lock;
    pthread_cond_t sched_id;
    void *data;
};

enum CoderState {
    COMPILE,
    DEBUGGING,
    REFACTOR,
};

// struct s_cturn {
//     int cturn;
//     pthread_mutex_t cturn_mutex;
// };

struct s_ArgvParsedConfig {
    unsigned int number_of_coders;
    unsigned int number_of_compiles_required;
    size_t time_to_burnout;
    size_t time_to_compile;
    size_t time_to_debug;
    size_t time_to_refactor;
    size_t dongle_cooldown;
    char scheduler[5];
};

extern struct s_ArgvParsedConfig DEFAULT_CONFIG;


struct s_CoderArg {
    size_t last_time_comp;
    size_t last_time_refact;
    size_t last_time_debug;
    unsigned int compiled_count;
};

struct s_CoderState {
    unsigned int id;
    unsigned int is_queued;
    enum CoderState state;
    struct s_UsbDongleState *l_usb;
    struct s_UsbDongleState *r_usb;
    struct s_CoderArg *arg;
    struct s_monitorstate *mstate;
    const struct s_globalstate *gconfig;
};

struct s_UsbDongleState {
    pthread_mutex_t usb_mutex;
    pthread_cond_t usb_rec_cond;
    int is_available;
    size_t cdown_start;
};

/* ===== ANSI COLORS ===== */
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED    "\033[31m"
#define COLOR_RESET  "\033[0m"

#ifdef DEBUG_MODE
#define DEBUG(fmt, ...) \
    do { \
        fprintf(stderr, COLOR_YELLOW "[DEBUG] %s:%d:%s(): " fmt COLOR_RESET "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)
#else
#define DEBUG(fmt, ...) ((void)0)
#endif

#define ERROR(fmt, ...) \
    do { \
        fprintf(stderr, COLOR_RED "[ERROR] %s:%d:%s(): " fmt COLOR_RESET "\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    } while (0)

#define FATAL(fmt, ...) \
do { \
    fprintf(stderr, "[FATAL] %s:%d:%s(): " fmt "\n", \
            __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    exit(EXIT_FAILURE); \
} while (0)

#ifdef TESTING
    #define RUN_TEST(code) do { code } while(0)
#else
    #define RUN_TEST(code) do {} while(0)
#endif

#define SAFE_PRINT(gstate, ...) do { \
    pthread_mutex_lock(&(gstate)->print_lock); \
    printf(__VA_ARGS__); \
    pthread_mutex_unlock(&(gstate)->print_lock); \
} while(0)

size_t get_curr_time_ms();
size_t check_time(struct s_CoderState *s_arg);
size_t get_abs_timeout_from_now_ms(size_t wait_ms, struct timespec *ts);

struct s_ArgvParsedConfig *create_config(int argc, char **argv);

void print_lock(struct s_globalstate *gstate, const char *fmt, ...);
void coder_thread_comp(struct s_CoderState *s_arg);
void mem_cleanup(struct s_globalstate *gstate);
void *coder_thread(void *arg);

int init_monitor_thread(struct s_globalstate *gstate);
int init_usb_mutexes_conds(int num_usb, struct s_UsbDongleState **usb_mutexes);
int init_scheduler(struct s_globalstate *gstate);
int argv_parser_validator(int argc, char **argv, struct s_ArgvParsedConfig *s);
int init_fifo_scheduler(struct s_globalstate *gstate);
int init_edf_scheduler(struct s_globalstate *gstate);
int  coder_thread_refactor(struct s_CoderState *s_arg);
int init_coder_threads(struct s_globalstate *gstate);
int  coder_thread_debug(struct s_CoderState *s_arg);
int take_usb(struct s_CoderState *s_arg, int direc);
int put_usb(struct s_CoderState *s_arg, int direc);
int check_death(struct s_CoderState *s_arg);


#endif
