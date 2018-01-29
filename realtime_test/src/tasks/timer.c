
//------------------------INCLUDES----------------------------------//
#include "timer.h"

//-------------------------DEFINES----------------------------//


//----------------------PUBLIC FUNCTIONS----------------------------//

int make_periodic(unsigned int period, periodic_info_Struct *info){
    
    //create the timer
    int fd = timerfd_create(CLOCK_MONOTONIC, 0);
    info->timer_fd = fd;
    info->wakeups_missed = 0;
    
    if(fd == -1){
        return fd;
    }

    //make the timer periodic
    unsigned int sec = period/1000;
    unsigned int ns = (period - (sec*1000))*1000000;

    struct itimerspec itval;
    itval.it_interval.tv_sec = sec;
    itval.it_interval.tv_nsec = ns;
	itval.it_value.tv_sec = sec;
	itval.it_value.tv_nsec = ns;
    
    return timerfd_settime(fd, 0, &itval, NULL);

}

void wait_period(periodic_info_Struct *info){

    //Wait for the next timer event. 
    //If we have missed any the number is written to "missed"    
    unsigned long long missed;
    int ret = read(info->timer_fd, &missed, sizeof(missed));
    if (ret == -1) {
		perror("read timer");
        return;
	}

	info->wakeups_missed += missed;

    /* "missed" should always be >= 1, but just to be sure, check it is not 0 anyway */
	if (missed > 0)
		info->wakeups_missed += (missed - 1);
        
}