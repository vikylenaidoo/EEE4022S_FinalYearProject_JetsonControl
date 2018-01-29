
//---------------------- INCLUDES --------------------------//
#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/timerfd.h>

//---------------------- DEFINES --------------------------//
typedef struct {
    int timer_fd;
    unsigned long long wakeups_missed;
} periodic_info_Struct;

typedef enum{
    TIMER_OK=0,
    TIMER_READ_ERROR
} Timer_Error_Typedef;

//---------------------- FUNCTIONS --------------------------//

/**
 * initiate the timer and set the period
 * input:   period:     period of the timer in ms
 * input:   *info:      ptr to periodic_info struct
 * return:    
 */
int make_periodic(unsigned int period, periodic_info_Struct *info);


/**
 * wait until the timer has triggered
 * input:   *info:      ptr to the periodic_info struct holding info on the timer
*/
void wait_period(periodic_info_Struct *info);

