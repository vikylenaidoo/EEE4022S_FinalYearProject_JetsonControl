
#include "test_uart.h"
static void terminate_handler(int sig_num);


int main(){
    printf("---------starting setup-------------\n");
    
    //gpio_port = gpio_init(78, GPIO_OUTPUT);

    //--------------starting uart---------------------
    //printf("---------starting uart-------------\n");
    int uart_error = uart_init(ttyUSB0, &serial_port_xbee);
    if(uart_error){
        printf("Error %i from uart_init: %s\n", uart_error, strerror(uart_error));
    }
    printf("serial port: %d\n", serial_port_xbee);

    //clear buffer
    memset(&xbee_read_buffer, '\0', sizeof(xbee_read_buffer));

    //termination interrupt
    signal(SIGINT, terminate_handler);

    //setup delay
    struct timespec deadline;
    deadline.tv_sec = 0;//1;//5;
    deadline.tv_nsec = 500000000;//500000000;//4000000;

    periodic_info_Struct xb_info;
    make_periodic(100, &xb_info); //4ms

    isRunning = 1;
    

    printf("---------starting test-------------\n");
    printf("size:   %ld\n", sizeof(xbee_read_buffer));
    while(isRunning){
        
        //gpio_set_value(gpio_port, GPIO_HIGH);

        
        memset(&xbee_read_buffer, '\0', sizeof(xbee_read_buffer));
        int num_bytes = uart_read(&serial_port_xbee, &xbee_read_buffer, sizeof(xbee_read_buffer));
        
        
        //printf("bytes: %d\n", num_bytes);
        if(num_bytes){
            printf("%s \n", xbee_read_buffer);

        }
        //printf("n\n");

       // gpio_set_value(gpio_port, GPIO_LOW);

       /* Sensor_Error_Typedef se = sensor_process_data(uart_read_buffer, (uint8_t)num_bytes);
        if(se){
            count_drops++;
            //printf("sensor status = %d \n", se);
            //printf("bytes: %d\n", num_bytes);
        }
        else{
            count_success++;

        }
        */
        //if(clock_nanosleep(CLOCK_MONOTONIC, 0, &deadline, NULL));
        wait_period(&xb_info);
        
    }

    close(serial_port_xbee);
    printf("serial port %d closed\n", serial_port_xbee);

    return 0;
    
}

/*
static void cleanupGPIO(){

    gpio_deinit(gpio_port, 78);
	printf("exiting gracefully\n");
    printf("dropped: %d\n", count_drops);
    printf("success: %d\n", count_success);
	isActive = 0;

}*/

static void terminate_handler(int sig_num){
	if(sig_num==SIGINT){
		//cleanupGPIO(gpio_port);
        isRunning=0;
	}
	else{
        printf("termination error\n");
        isRunning = 0;
    }
}
