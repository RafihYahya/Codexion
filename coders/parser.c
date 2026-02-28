#include "main.h"


struct s_ArgvParsedConfig DEFAULT_CONFIG = {
        .number_of_coders = 4,
        .time_to_burnout = 800,
        .time_to_compile = 200,
        .time_to_debug = 200,
        .time_to_refactor = 200,
        .number_of_compiles_required = 3,
        .dongle_cooldown = 50,
        //.cturn.cturn = 0,
        //.cturn.cturn_mutex = PTHREAD_MUTEX_INITIALIZER,
        .scheduler = "fifo"
        };

struct s_ArgvParsedConfig *create_config(int argc, char **argv)
{
    struct s_ArgvParsedConfig *cfg;

    cfg = malloc(sizeof(*cfg));
    if (!cfg)
        return (NULL);

    #ifdef SKIP_PARSE
    *cfg = DEFAULT_CONFIG;
    #else
    if (argv_parser_validator(argc, argv, cfg) < 0)
        {
            ERROR("Parsing Error");
            free(cfg);
            return NULL;
        }
    #endif
    return (cfg);
}

int argv_parser_validator(int argc, char **argv, struct s_ArgvParsedConfig *s)
{
	return 0;
}


