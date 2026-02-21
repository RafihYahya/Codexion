#include "mutex.h"

enum CoderState {
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
};


int init_coder_threads();

