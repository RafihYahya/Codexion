#ifndef MAIN_H
    #define MAIN_H

#include <unistd.h>
#include <stdlib.h>
#include "thread.h"
#include "mutex.h"
#include <stdio.h>
#include "parser.h"

/* ===== ANSI COLORS ===== */
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED    "\033[31m"
#define COLOR_RESET  "\033[0m"


struct s_monitorstate {
    int is_someone_dead;
    pthread_mutex_t death_lock;
};

struct s_globalstate {
    struct s_ArgvParsedConfig   *pconfig;
    struct s_UsbDongleState     *states;
    struct s_CoderState         *cstates;
    struct s_monitorstate       *mstate;
    struct s_scheduler          *scheduler;
    pthread_t                   *thd;
};


#ifdef DEBUG
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


#endif
