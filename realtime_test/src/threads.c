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

    //setup logging files
    fp_data_log = fopen("data/data_log", "wb");
    if(fp_data_log==NULL){
        printf("error opening data_log file\n");
    }

    //timestamp for logging
    struct timespec timestamp;

    //setup timer
    printf("----------------timer init---------------- \n");    
    periodic_info_Struct info;
    make_periodic(4, &info); //4ms

    //begin action
    while(isActive){ 

        //request data
        if(gpio_set_value(fp_data_rq, GPIO_HIGH)){
            printf("error\n");
        }

        //read uart
        int num_bytes = uart_read(&serial_port_data, &uart_read_buffer, sizeof(uart_read_buffer));
        //printf("bytes: %d\n", num_bytes);

        //update timestamp
        clock_gettime(CLOCK_REALTIME, &timestamp);

        //process sensor data and pack into struct
        Sensor_Error_Typedef se = sensor_process_data(uart_read_buffer, (uint8_t)num_bytes);
        
        if(se){
            count_drops++;
            //printf("sensor status = %d \n", se);
            //printf("bytes: %d\n", num_bytes);
        }
        else{
            count_success++;
            
            //log data
            if(fp_data_log != NULL){
                //fseek(fp_sensor_log, 0, SEEK_SET);
                int wb = fwrite(&(timestamp), sizeof(timestamp), 1, fp_data_log); //log timestamp (16 bytes)
                wb = fwrite(uart_read_buffer, num_bytes, 1, fp_data_log); //log actual data
                //total size per block = 8+8+132=148
                
            }
        }

        gpio_set_value(fp_data_rq, GPIO_LOW);


        //wait until next period
        wait_period(&info);
        time_counter++;
    }
    
    //close port
    close(serial_port_data);
    printf("serial port %d closed\n", serial_port_data);

    //cleanup gpio
    gpio_deinit(fp_data_rq, pin_data_rq);
	printf("---------exiting gracefully----------------\n");
    printf("dropped: %d\n", count_drops);
    printf("success: %d\n", count_success);
    printf("time: %d ms\n", time_counter*4);

    //close log files
    if(fp_data_log != NULL)
        fclose(fp_data_log);

    return 0;
}

/**
 * xbee telemetry thread. receive remote commands from user
 * and send some onboard data back to be displayed
*/
int thread_xbee_telemetry(){
    //starting uart
    int uart_error = uart_init(ttyUSB0, &serial_port_xbee);
    if(uart_error){
        printf("Error %i from uart_init: %s\n", uart_error, strerror(uart_error));
        //isActive=0;
        //return 1;
    }
    printf("xbee on serial port: %d\n", serial_port_xbee);

    //clear buffer
    memset(&xbee_read_buffer, '\0', sizeof(xbee_read_buffer));


    //setup timer
    periodic_info_Struct xb_info;
    make_periodic(50, &xb_info); //4ms

    int send_counter = 0;

    while(isActive){

        //read serial data
        memset(&xbee_read_buffer, '\0', sizeof(xbee_read_buffer));
        int num_bytes = uart_read(&serial_port_xbee, &xbee_read_buffer, sizeof(xbee_read_buffer)); //sizeof(xbee_read_buffer)
        //int num_bytes =0;

        int isMessageValid = 0;

        //check for heartbeat or message
        //printf("%d bytes read from xb: %s \n", num_bytes, xbee_read_buffer);
        
        if(num_bytes==10){ //valid data
            
            //xb_drop=0;

            //if(num_bytes==10){ //valid data
                union four_bytes{
                    uint32_t value;
                    uint8_t bytes[4];
                } union_four_bytes;

                uint8_t message_type = xbee_read_buffer[1];
                
                //get the message data
                for (int i = 0; i < 4; i++){
                    union_four_bytes.bytes[3-i] = xbee_read_buffer[i+2];
                }                
                uint32_t message_data = union_four_bytes.value;
                
                //get the message crc32
                for (int i = 0; i < 4; i++){
                    union_four_bytes.bytes[3-i] = xbee_read_buffer[i+6];
                }
                uint32_t message_crc = union_four_bytes.value;

                //printf("%d \t %d \t %x \n", message_type, message_data, message_crc);
            //}

            //validate checksum
            uint32_t crc32_1592 = crc32_1592_calculate(&xbee_read_buffer[1], 5);
            if(crc32_1592 == message_crc){
                isMessageValid = 1;
                xb_drop = 0; //reset drop count
                //printf("%x \n", crc32_1592);
            }
            else{
                printf("invalid data \n");
            }
            
        }
        else{
            //usleep(1000);
            //tcflush(serial_port_xbee, TCIFLUSH);
            //printf("flush\n");
        }

        if(!isMessageValid){ //heartbeat dropped
            if(xb_drop){
                xb_drop++;
            }
            else{
                xb_drop = 1;
            }
        }

        if(xb_drop>5){
            printf("dropped: %d\n", xb_drop);
        }

        //emergency kill
        if(xb_drop>20){ //more than 1s of dropped pings 
            //isActive = 0;
            xb_drop=0;
            printf("----------emergency kill-----------\n");
        }
        

        //send monitoring data back to xbee
        
        /*
        if(send_counter == 20){
            //send data to xbee
            char message[] = {'h'};
            
            uart_write(&serial_port_xbee, message, sizeof(message));
        }
        send_counter++;
        */


        //wait for next period
        wait_period(&xb_info);
    }

    //close the port after application ended
    close(serial_port_xbee);
    printf("serial port %d closed\n", serial_port_xbee);



    return 0;
} 





//------------------------AUX FNS--------------------------------------//
