#ifndef MAIN_H
    #define MAIN_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/* ===== ANSI COLORS ===== */
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED    "\033[31m"
#define COLOR_RESET  "\033[0m"


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

struct s_ArgvParsedConfig {
    int number_of_coders;
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles_required;
    int dongle_cooldown;
    char scheduler[3];
};

int argv_parser_validator(int argc, char **argv, struct s_ArgvParsedConfig *s);

#endif
