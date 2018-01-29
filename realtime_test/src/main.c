
#include "main.h"

static void terminate_handler(int sig_num);
//static FILE* test_fp;
//static int isActive = 1;
//#define test_pin 78

//------------- THREADS----------------------//

void *thread_function_rt_control(void *data){
	printf("rt thread created!!!\n");
	thread_rt_control();


	printf("----------------rt thread ended!!!------------------\n");
	return NULL;
}

void *thread_function_xbee_telemetry(void *data){
	printf("xb thread created!!!\n");
	thread_xbee_telemetry();


	printf("-----------------xb thread ended!!!---------------\n");
	return NULL;
}

//----------------MAIN FUNCTION-----------------------//
int main(int argc, char *argv[]){ 

	//create termination interrupt
	signal(SIGINT, terminate_handler);

	//set isActive flag for all threads to be running
	isActive = 1; //set to 0 to end program

	
	//vars for threads setup
	struct sched_param rt_param;
	pthread_attr_t rt_attr;
	pthread_t rt_control_pthread;

	struct sched_param xb_param;
	pthread_attr_t xb_attr;
	pthread_t xb_telemetry_pthread;

	int ret;
	

	
	//lock memory
	if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1){
		printf("mlockall failed: %m\n");
		exit(-2);
	}
	
	//-------------------- rt control thread ---------------//

	//initialise pthread attribtues (default values)
	ret = pthread_attr_init(&rt_attr);
	if(ret){
		printf("init pthread attributes failed \n");
		return ret;
	}
	
	//set a specific stack size
	ret = pthread_attr_setstacksize(&rt_attr, PTHREAD_STACK_MIN);
	if(ret){
		printf("pthread setstacksize failed \n");
		return ret;
	}
	
	//set scheduler policy and priority of pthread
	ret = pthread_attr_setschedpolicy(&rt_attr, SCHED_FIFO); 
	if(ret){
		printf("pthread setschedpolicy failed \n");
		return ret;
	}
	rt_param.sched_priority = 98;
	ret = pthread_attr_setschedparam(&rt_attr, &rt_param);
	if(ret){
		printf("pthread setschedparam failed \n");
		return ret;
	}
	
	//use sceduling parameters of attr
	ret = pthread_attr_setinheritsched(&rt_attr, PTHREAD_EXPLICIT_SCHED);
	if(ret){
		printf("pthread inheritsched failed \n");
		return ret;
	}
	
	
	
	//-------------xbee telemetry thread-------------------//
	
	//initialise pthread attribtues (default values)
	ret = pthread_attr_init(&xb_attr);
	if(ret){
		printf("init pthread attributes failed \n");
		return ret;
	}
	
	//set a specific stack size
	ret = pthread_attr_setstacksize(&xb_attr, PTHREAD_STACK_MIN);
	if(ret){
		printf("pthread setstacksize failed \n");
		return ret;
	}
	
	//set scheduler policy and priority of pthread
	ret = pthread_attr_setschedpolicy(&xb_attr, SCHED_FIFO); 
	if(ret){
		printf("pthread setschedpolicy failed \n");
		return ret;
	}
	xb_param.sched_priority = 95;
	ret = pthread_attr_setschedparam(&xb_attr, &xb_param);
	if(ret){
		printf("xb pthread setschedparam failed \n");
		return ret;
	}
	
	//use sceduling parameters of attr
	ret = pthread_attr_setinheritsched(&xb_attr, PTHREAD_EXPLICIT_SCHED);
	if(ret){
		printf("pthread inheritsched failed \n");
		return ret;
	}
	
	//-----------start all threads---------------------//

	//create rt control pthread with specified attributes
	ret = pthread_create(&rt_control_pthread, &rt_attr, thread_function_rt_control, NULL);
	if(ret){
		printf("rt control pthread create failed \n");
		return ret;
	}

	//create rt control pthread with specified attributes
	ret = pthread_create(&xb_telemetry_pthread, &xb_attr, thread_function_xbee_telemetry, NULL);
	if(ret){
		printf("xb telemetry pthread create failed \n");
		return ret;
	}

	//-----------now threads are running. Join when finished------------------//


	//join the threads and wait until done
	ret = pthread_join(rt_control_pthread, NULL);
	if(ret){
		printf("join rt thread failed \n");
		return ret;
	}
	
	//join the threads and wait until done
	ret = pthread_join(xb_telemetry_pthread, NULL);
	if(ret){
		printf("join xb failed \n");
		return ret;
	}
	
	
	
	printf("-------------program ended-------------\n");
	return 0;	
}


//------------------ AUX FUNCTIONS ------------------//


static void terminate_handler(int sig_num){
	if(sig_num==SIGINT){
		isActive = 0;
		//cleanupGPIO(pin_data_rq);
	}
	else{
        printf("termination error, terminating anyways\n");
        isActive = 0;
    }
}

