#include "mutex.h"

int init_usb_mutexes(int num_usb, struct s_UsbDongleState *usb_mutexes)
{
    int i = 0;
    int err = 0;

    if (!usb_mutexes)
    {
        return (-1);
    }
    while (i < num_usb)
    {
        err = pthread_mutex_init(&usb_mutexes[i].usb_mutex, NULL);
        if (err < 0)
        {
            free(usb_mutexes);
            return (-1);
        }
        usb_mutexes[i].usb_cooldwon = -1;
        i++;
    }

    return (0);
}