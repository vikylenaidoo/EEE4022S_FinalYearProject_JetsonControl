
#include "main.h"

static void terminate_handler(int sig_num);
//static FILE* test_fp;
//static int isActive = 1;
//#define test_pin 78

//------------- THREADS----------------------//

void *thread_function(void *data){
	printf("thread created!!!\n");
	test_function();
	/*
    test_fp = gpio_init(test_pin, GPIO_OUTPUT);
    GPIO_IO_Value_Typedef status = GPIO_LOW;
    signal(SIGINT, terminate_handler);
    
	struct timespec deadline;
	//deadline.tv_sec = 0;
	//deadline.tv_nsec = 400000;
   
    printf("starting test sequence !!!\n");

	while (isActive){
        clock_gettime(CLOCK_MONOTONIC, &deadline);
        deadline.tv_nsec += 4000000;

        if(clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL)){
            //printf("sleep error !!!\n");
        }

        if(gpio_set_value(test_fp, status)){
            printf("error\n");
        }

        if(status==GPIO_LOW){
            status =GPIO_HIGH;
        }
        else{
            status = GPIO_LOW;
        }

    }

	*/


	printf("thread ended!!!\n");
	return NULL;
}

//----------------MAIN FUNCTION-----------------------//
int main(int argc, char *argv[]){ 
	
	struct sched_param param;
	pthread_attr_t attr;
	pthread_t thread;
	int ret;
	
	//lock memory
	if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1){
		printf("mlockall failed: %m\n");
		exit(-2);
	}
	
	//initialise pthread attribtues (default values)
	ret = pthread_attr_init(&attr);
	if(ret){
		printf("init pthread attributes failed \n");
		return ret;
	}
	
	//set a specific stack size
	ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
	if(ret){
		printf("pthread setstacksize failed \n");
		return ret;
	}
	
	//set scheduler policy and priority of pthread
	ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO); 
	if(ret){
		printf("pthread setschedpolicy failed \n");
		return ret;
	}
	param.sched_priority = 99;
	ret = pthread_attr_setschedparam(&attr, &param);
	if(ret){
		printf("pthread setschedparam failed \n");
		return ret;
	}
	
	//use sceduling parameters of attr
	ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	if(ret){
		printf("pthread inheritsched failed \n");
		return ret;
	}
	
	//create pthread with specified attributes
	ret = pthread_create(&thread, &attr, thread_function, NULL);
	if(ret){
		printf("pthread create failed \n");
		return ret;
	}
	
	
	//join the threads and wait until done
	ret = pthread_join(thread, NULL);
	if(ret){
		printf("pthread setschedparam failed \n");
		return ret;
	}
	
	
	
	
	printf("-------------program ended-------------\n");
	return 0;	
}


//------------------ AUX FUNCTIONS ------------------//
