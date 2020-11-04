#include "threads.h"


//--------------------THREADS-----------------//

/**
 * Real time control thread
*/

int thread_rt_control(){
    
    //initialise gpio for data requesting
    printf("----------------gpio init---------------- \n");    
    fp_data_rq = gpio_init(pin_data_rq, GPIO_OUTPUT);

    //setup uart for sensor data comms
    printf("---------------- sensor uart init---------------- \n");    
    int uart_error = uart_init(ttyTHS1, &serial_port_data);
    if(uart_error){
        printf("Error %i from sensor uart init: %s\n", uart_error, strerror(uart_error));
    }
    printf("sensor serial port opened: %d\n", serial_port_data);

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
    make_periodic(RT_PERIOD, &info); //4ms

    //control setup
    isAlive = 0; //initially no control action
    controlMode = 0;   //initially manual mode

    //setup pololu servo motors
    printf("---------------- pololu servo driver init---------------- \n");    
    uart_error = uart_init(ttyACM0, &serial_port_servo);
    if(uart_error){
        printf("Error %i from servo init: %s\n", uart_error, strerror(uart_error));
    }
    printf("servo serial port opened: %d\n", serial_port_servo);
    
    //disable all servo outputs to zero
    servo_disable(&serial_port_servo, 0);
    servo_disable(&serial_port_servo, 1);
    servo_disable(&serial_port_servo, 2);
    servo_disable(&serial_port_servo, 3);
    servo_disable(&serial_port_servo, 4);
    servo_disable(&serial_port_servo, 5);
    

    //begin action
    while(isRunning){ 

        //request data
        if(gpio_set_value(fp_data_rq, GPIO_HIGH)){
            printf("error\n");
        }

        //read uart
        int num_bytes = uart_read(&serial_port_data, &uart_read_buffer, sizeof(uart_read_buffer));
        //printf("bytes: %d\n", num_bytes);

        //update timestamp
        clock_gettime(CLOCK_MONOTONIC, &timestamp); //CLOCK_REALTIME

        //process sensor data and pack into struct
        Sensor_Error_Typedef se = sensor_process_data(uart_read_buffer, (uint8_t)num_bytes);
        
        if(se){
            count_drops++;
            //printf("sensor status = %d \n", se);
            //printf("bytes: %d\n", num_bytes);
        }
        else{
            count_success++;

            //update control action
            
            if(isAlive){ 
                if(controlMode==0){ //normal mode
                    

                }
                else{ //auto control mode
                    //@todo

                }
            }
            else{ //disable all motion
                //throttle off
                //reset steering
                //reset tail
            }
            
            //log data
            if(fp_data_log != NULL){
                //fseek(fp_sensor_log, 0, SEEK_SET);
                int wb = fwrite(&(timestamp), sizeof(timestamp), 1, fp_data_log); //log timestamp (16 bytes)
                wb = fwrite(uart_read_buffer, num_bytes, 1, fp_data_log); //log actual data
                //total size per block = 8+8+132=148
                
            }

            //send certain data to xbee fro ui display
            //@todo

        }

        gpio_set_value(fp_data_rq, GPIO_LOW);


        //wait until next period
        wait_period(&info);
        time_counter++;
    }
    
    //close port
    close(serial_port_data);
    printf("serial port %d closed\n", serial_port_data);

    //kill motors
    servo_disable(&serial_port_servo, 0);
    servo_disable(&serial_port_servo, 1);
    servo_disable(&serial_port_servo, 2);
    close(serial_port_servo);
    printf("serial port %d closed\n", serial_port_servo);

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


//---------------------------------XBEE THREAD-------------------------------//
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

    while(isRunning){

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

                //perform required function
                Servo_Error_Typedef e = SERVO_OK;
                int pwm_value;
                switch(message_type){
                    case 0:     //heartbeat
                        break;

                    case 1:     //kill
                        isAlive = 0;
                        printf("---------------kill-----------\n");
                        servo_set_target(&serial_port_servo, 0, 1500);
                        e = servo_disable(&serial_port_servo, 0);
                        if(e){
                            printf("---------------servo error %d-----------\n", e);
                        } 
                        servo_set_target(&serial_port_servo, 1, 1500);
                        e = servo_disable(&serial_port_servo, 1);
                        if(e){
                            printf("---------------servo error %d-----------\n", e);
                        } 
                        servo_set_target(&serial_port_servo, 1, 1500);
                        e = servo_disable(&serial_port_servo, 2);
                        if(e){
                            printf("---------------servo error %d-----------\n", e);
                        } 
                        break;
                     
                    case 2:     //revive
                        isAlive = 1;
                        printf("---------------revive-----------\n");                                               
                        break;

                    case 3:     //throttle
                        printf("---------------throttle-----------\n");
                        controlMode = 0; //manual
                        if(isAlive){
                            pwm_value = (unsigned int)(((double)message_data/100.0)*(MAX_WIDTH_CHANNEL_0-MIN_WIDTH_CHANNEL_0) + MIN_WIDTH_CHANNEL_0);
                            e = servo_set_target(&serial_port_servo, 0, pwm_value);
                            if(e){
                                printf("---------------servo error %d-----------\n", e);
                            }
                        }
                        break;   
                    
                    case 4:     //steering
                        printf("---------------steering-----------\n");
                        controlMode = 0; //manual
                        if(isAlive){
                            pwm_value = (unsigned int)(((double)message_data/100.0)*(MAX_WIDTH_CHANNEL_1-MIN_WIDTH_CHANNEL_1) + MIN_WIDTH_CHANNEL_1);
                            e = servo_set_target(&serial_port_servo, 1, pwm_value);
                            printf("pwm width: %d \n", pwm_value);
                            if(e){
                                printf("---------------servo error %d-----------\n", e);
                            }
                        }
                        break;
                        
                    case 5:     //tail
                        printf("---------------throttle-----------\n");
                        controlMode = 0; //manual
                        if(isAlive){
                            pwm_value = ((int)(message_data/100))*(MAX_WIDTH_CHANNEL_2-MIN_WIDTH_CHANNEL_2) + MIN_WIDTH_CHANNEL_2;
                            e = servo_set_target(&serial_port_servo, 2, pwm_value);
                            if(e){
                                printf("---------------servo error %d-----------\n", e);
                            }
                        }
                        break;

                    default:
                        break;                                                                

                }

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
            isAlive = 0;
            xb_drop=0;
            printf("----------emergency kill-----------\n");
            servo_disable(&serial_port_servo, 0);
            servo_disable(&serial_port_servo, 1);
            servo_disable(&serial_port_servo, 2);
            
        }
        

        //send monitoring data back to xbee
        if(send_counter >= 5){
            //send data to xbee
            uint8_t message[68]; //28sensor+36gnss+4crc=68 bytes
            

            //everything in sensorData_struct
            union{
                uint8_t bytes[sizeof(Global_Sensor_Data)];
                struct Sensor_Data_Struct data_struct;
            } sensor_data_union;

            sensor_data_union.data_struct = Global_Sensor_Data;
            int i=0;
            for(i; i<sizeof(Global_Sensor_Data); i++){
                message[i] = sensor_data_union.bytes[i];
            }
            

            //specific gnss data
            union{
                uint8_t bytes[4];
                uint32_t value;
            } four_bytes_union;
            
            four_bytes_union.value = Global_GNSS_Data.GNSS_lat;
            for(int j=0; j<4; j++){
                message[i] = four_bytes_union.bytes[j];
                i++;
            }

            four_bytes_union.value = Global_GNSS_Data.GNSS_lon;
            for(int j=0; j<4; j++){
                message[i] = four_bytes_union.bytes[j];
                i++;
            }

            four_bytes_union.value = Global_GNSS_Data.GNSS_velN;
            for(int j=0; j<4; j++){
                message[i] = four_bytes_union.bytes[j];
                i++;
            }

            four_bytes_union.value = Global_GNSS_Data.GNSS_velE;
            for(int j=0; j<4; j++){
                message[i] = four_bytes_union.bytes[j];
                i++;
            }

            four_bytes_union.value = Global_GNSS_Data.GNSS_velD;
            for(int j=0; j<4; j++){
                message[i] = four_bytes_union.bytes[j];
                i++;
            }

            four_bytes_union.value = Global_GNSS_Data.GNSS_height;
            for(int j=0; j<4; j++){
                message[i] = four_bytes_union.bytes[j];
                i++;
            }
            
            four_bytes_union.value = Global_GNSS_Data.GNSS_hMSL;
            for(int j=0; j<4; j++){
                message[i] = four_bytes_union.bytes[j];
                i++;
            }

            four_bytes_union.value = Global_GNSS_Data.GNSS_gSpeed;
            for(int j=0; j<4; j++){
                message[i] = four_bytes_union.bytes[j];
                i++;
            }

            four_bytes_union.value = Global_GNSS_Data.GNSS_headMot;
            for(int j=0; j<4; j++){
                message[i] = four_bytes_union.bytes[j];
                i++;
            }

            //calculate crc32
            uint32_t crc_telemetry_send = crc32_1592_calculate(message, 64);
            //printf("crc: %x \n", crc_telemetry_send);
            four_bytes_union.value = crc_telemetry_send;
            for(int j=0; j<4; j++){
                message[i] = four_bytes_union.bytes[j];
                i++;
            }

            //printf("%x %x %x %x  \n", message[64], message[65], message[66], message[67]);
            //write the data
            if(serial_port_xbee != -1){
                uart_write(&serial_port_xbee, message, sizeof(message));
            }
            
            
            send_counter=0;
        }
        else{
            send_counter++;
        }


        //wait for next period
        wait_period(&xb_info);
    }

    //close the port after application ended
    close(serial_port_xbee);
    printf("serial port %d closed\n", serial_port_xbee);



    return 0;
} 





//------------------------AUX FNS--------------------------------------//
