#include <pthread.h>

struct s_UsbDongleState {
    pthread_mutex_t usb_mutex;
    unsigned long recov_time;
};

int init_usb_mutexes(int num_usb, struct s_UsbDongleState **usb_mutexes);


