
#include "test.h"

static void terminate_handler(int sig_num);

int test_function(){

    printf("----------------gpio init---------------- \n");
    
    test_fp = gpio_init(test_pin, GPIO_OUTPUT);

    GPIO_IO_Value_Typedef status = GPIO_LOW;

    signal(SIGINT, terminate_handler);

    /*struct timespec deadline;
    deadline.tv_sec = 0;
    deadline.tv_nsec = 4000000;
    */

    //setup timer
    periodic_info_Struct info;
    make_periodic(4, &info); //4ms
    

    printf("------------starting test sequence !!!-------------\n");
    
    while (isActive){
        
        /*
        if(clock_nanosleep(CLOCK_MONOTONIC, 0, &deadline, NULL)){
            //printf("sleep error !!!\n");
        }*/


        //perform function
        if(gpio_set_value(test_fp, status)){
            printf("error\n");
        }

        if(status==GPIO_LOW){
            status =GPIO_HIGH;
        }
        else{
            status = GPIO_LOW;
        }

        //wait until next period
        wait_period(&info);
        //printf("r:\t%d\n", r);
        //printf("missed: \t %d \n", info.wakeups_missed);

    }
    


    return 0;
}

static void cleanupGPIO(){

    gpio_deinit(test_fp, 78);
	printf("exiting gracefully\n");

	isActive = 0;

}

static void terminate_handler(int sig_num){
	if(sig_num==SIGINT){
		cleanupGPIO(test_fp);
	}
	else{
        printf("termination error\n");
        isActive = 0;
    }
}
