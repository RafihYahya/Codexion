#include "main.h"

enum CoderState {
    STANDBY,
    COMPILE,
    DEBUG,
    REFACTOR,
    BURNOUT
};

struct s_CoderArg {
    unsigned long time_awake;
    unsigned long compiled_count;
};

struct s_CoderState {
    unsigned int id;
    enum CoderState state;
    struct s_UsbDongleState *l_usb;
    struct s_UsbDongleState *r_usb;
    struct s_CoderArg *arg;
    const struct s_ArgvParsedConfig *gconfig;
};


int init_coder_threads(struct s_ArgvParsedConfig *arg, struct s_CoderState **cstates,
    struct s_UsbDongleState **mutexes,  pthread_t **thd);
