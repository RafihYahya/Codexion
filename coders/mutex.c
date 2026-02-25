#include "main.h"

int init_usb_mutexes(int num_usb, struct s_UsbDongleState **usb_mutexes)
{
    int i = 0;
    int err = 0;

    if (!usb_mutexes)
    {
        ERROR("Must Not be Null ");
        return (-2);
    }
    *usb_mutexes = malloc(sizeof(struct s_UsbDongleState) * num_usb);
    if (!*usb_mutexes)
    {
        ERROR("Malloc Has Failed, Just like most of your dreams");
        return (-1);
    }
    DEBUG("Initiating Mutex");
    while (i < num_usb)
    {
        err = pthread_mutex_init(&(*usb_mutexes[i]).usb_mutex, NULL);
        if (err < 0)
        {
            ERROR("Couldn't Fully Init Mutexes");
            free(usb_mutexes);
            return (-1);
        }
        (*usb_mutexes[i]).recov_time = -1;
        i++;
    }
    DEBUG(usb_mutexes);
    return (0);
}