
#include "test_uart.h"
static void terminate_handler(int sig_num);


int main(){
    printf("---------starting setup-------------\n");
    
    gpio_port = gpio_init(78, GPIO_OUTPUT);

    //printf("---------starting uart-------------\n");
    int uart_error = uart_init(ttyTHS1, &serial_port);
    if(uart_error){
        printf("Error %i from uart_init: %s\n", uart_error, strerror(uart_error));
    }
    printf("serial port: %d\n", serial_port);

    memset(&uart_read_buffer, '\0', sizeof(uart_read_buffer));


    signal(SIGINT, terminate_handler);

    struct timespec deadline;
    deadline.tv_sec = 5;
    deadline.tv_nsec = 0;//500000000;//4000000;
    
    isActive = 1;

    printf("---------starting test-------------\n");
    printf("size:   %ld\n", sizeof(uart_read_buffer));
    while(isActive){
        
        gpio_set_value(gpio_port, GPIO_HIGH);
        int num_bytes = uart_read(&serial_port, &uart_read_buffer, sizeof(uart_read_buffer));
        //int num_bytes = read(serial_port, &uart_read_buffer, sizeof(uart_read_buffer));
        
        printf("bytes: %d\n", num_bytes);

        gpio_set_value(gpio_port, GPIO_LOW);

        Sensor_Error_Typedef se = sensor_process_data(uart_read_buffer, (uint8_t)num_bytes);
        printf("sensor status = %d \n", se);

        if(clock_nanosleep(CLOCK_MONOTONIC, 0, &deadline, NULL)){
            //printf("sleep error !!!\n");
        }
        
    }

    return 0;
    
}

static void cleanupGPIO(){

    gpio_deinit(gpio_port, 78);
	printf("exiting gracefully\n");

	isActive = 0;

}

static void terminate_handler(int sig_num){
	if(sig_num==SIGINT){
		cleanupGPIO(gpio_port);
	}
	else{
        printf("termination error\n");
        isActive = 0;
    }
}
