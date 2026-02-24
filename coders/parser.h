#ifndef PARSER_H
 #define PARSER_H

struct s_cturn {
    int cturn;
    pthread_mutex_t cturn_mutex;
};

struct s_ArgvParsedConfig {
    const int number_of_coders;
    const int time_to_burnout;
    const int time_to_compile;
    const int time_to_debug;
    const int time_to_refactor;
    const int number_of_compiles_required;
    const int dongle_cooldown;
    struct s_cturn cturn;
    const char scheduler[5];
};

static const struct s_ArgvParsedConfig DEFAULT_CONFIG = {
        .number_of_coders = 4,
        .time_to_burnout = 480,
        .time_to_compile = 30,
        .time_to_debug = 45,
        .time_to_refactor = 60,
        .number_of_compiles_required = 10,
        .dongle_cooldown = 5,
        .cturn.cturn = 0,
        .cturn.cturn_mutex = PTHREAD_MUTEX_INITIALIZER,
        .scheduler = "fifo"
        };

int argv_parser_validator(int argc, char **argv, struct s_ArgvParsedConfig *s);


#endif
