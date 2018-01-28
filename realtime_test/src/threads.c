#include "threads.h"


//--------------------THREADS-----------------//

/**
 * Real time control thread
*/

int thread_rt_control(){
    
    //initialise gpio for data requesting
    printf("----------------gpio init---------------- \n");    
    fp_data_rq = gpio_init(pin_data_rq, GPIO_OUTPUT);

    //setup uart
    printf("----------------uart init---------------- \n");    
    int uart_error = uart_init(ttyTHS1, &serial_port_data);
    if(uart_error){
        printf("Error %i from uart_init: %s\n", uart_error, strerror(uart_error));
    }
    printf("serial port opened: %d\n", serial_port_data);

    //clear uart recieve buffer
    memset(&uart_read_buffer, '\0', sizeof(uart_read_buffer));

    //setup timer
    printf("----------------timer init---------------- \n");    
    periodic_info_Struct info;
    make_periodic(4, &info); //4ms


    while(isActive){

        //request data
        if(gpio_set_value(fp_data_rq, GPIO_HIGH)){
            printf("error\n");
        }

        //read uart
        int num_bytes = uart_read(&serial_port_data, &uart_read_buffer, sizeof(uart_read_buffer));
        //printf("bytes: %d\n", num_bytes);

        

        //process sensor data and pack into struct
        Sensor_Error_Typedef se = sensor_process_data(uart_read_buffer, (uint8_t)num_bytes);
        
        if(se){
            count_drops++;
            //printf("sensor status = %d \n", se);
            //printf("bytes: %d\n", num_bytes);
        }
        else{
            count_success++;

        }

        gpio_set_value(fp_data_rq, GPIO_LOW);

        //wait until next period
        wait_period(&info);
    }
    
    //close port
    close(serial_port_data);
    printf("serial port %d closed\n", serial_port_data);

    //cleanup gpio
    gpio_deinit(fp_data_rq, pin_data_rq);
	printf("---------exiting gracefully----------------\n");
    printf("dropped: %d\n", count_drops);
    printf("success: %d\n", count_success);

    return 0;
}

/**
 * xbee telemetry thread. receive remote commands from user
*/
int thread_xbee_telemetry(){
    //starting uart
    int uart_error = uart_init(ttyUSB0, &serial_port_xbee);
    if(uart_error){
        printf("Error %i from uart_init: %s\n", uart_error, strerror(uart_error));
    }
    printf("serial port: %d\n", serial_port_xbee);

    //clear buffer
    memset(&xbee_read_buffer, '\0', sizeof(xbee_read_buffer));

    //setup delay
    struct timespec deadline;
    deadline.tv_sec = 0;//1;//5;
    deadline.tv_nsec = 500000000;//500000000;//4000000;

    //setup timer
    periodic_info_Struct xb_info;
    make_periodic(100, &xb_info); //4ms

    while(isActive){

        //read serial data
        memset(&xbee_read_buffer, '\0', sizeof(xbee_read_buffer));
/*        int num_bytes = uart_read(&serial_port_xbee, &xbee_read_buffer, sizeof(xbee_read_buffer));

        if(num_bytes){
            printf("%s \n", xbee_read_buffer);
            //xb_drop++;
        }
*/
        printf("x\n");
        /*
        if(xb_drop>100){ //emergency kill
            isActive = 0;
        }
        */

        //send data to xbee
        char message[] = {'h'};
        
        //uart_write(&serial_port_xbee, message, sizeof(message));

        //wait for next period
        wait_period(&xb_info);
        //clock_nanosleep(CLOCK_MONOTONIC, 0, &deadline, NULL);
    }

    //close the port after application ended
    close(serial_port_xbee);
    printf("serial port %d closed\n", serial_port_xbee);

    return 0;
} 





//------------------------AUX FNS--------------------------------------//
