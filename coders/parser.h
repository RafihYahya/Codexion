#ifndef PARSER_H
 #define PARSER_H

struct s_ArgvParsedConfig {
    int number_of_coders;
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles_required;
    int dongle_cooldown;
    char scheduler[5];
};

static const struct s_ArgvParsedConfig DEFAULT_CONFIG = {
        .number_of_coders = 4,
        .time_to_burnout = 480,
        .time_to_compile = 30,
        .time_to_debug = 45,
        .time_to_refactor = 60,
        .number_of_compiles_required = 10,
        .dongle_cooldown = 5,
        .scheduler = "fifo"
        };

int argv_parser_validator(int argc, char **argv, struct s_ArgvParsedConfig *s);


#endif
