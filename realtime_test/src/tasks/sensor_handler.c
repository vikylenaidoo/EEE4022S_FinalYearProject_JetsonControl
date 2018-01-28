/**
 * sensor_handler.h
 * used for decoding the data received from the sensor
*/

#include "sensor_handler.h"

//---------------------------DEFINES--------------------------//

//---------------------------STATIC FUNCTIONS--------------------------//


//---------------------------PUBLIC FUNCTIONS--------------------------//


Sensor_Error_Typedef sensor_process_data(uint8_t input_buffer[LENGTH_SENSOR_DATA], uint8_t length){
    
    /*----------------------data validation------------------*/
    //validate length
    if(length != LENGTH_SENSOR_DATA)
        return SENSOR_LENGTH;
    
    //get checksum from input data
    union crc32{
        uint32_t checksum;
        uint8_t bytes[4];
    } union_crc32;

    union_crc32.bytes[0] = input_buffer[LENGTH_SENSOR_DATA-4];
    union_crc32.bytes[1] = input_buffer[LENGTH_SENSOR_DATA-3];
    union_crc32.bytes[2] = input_buffer[LENGTH_SENSOR_DATA-2];
    union_crc32.bytes[3] = input_buffer[LENGTH_SENSOR_DATA-1];


    //calculate crc for this input data
    uint32_t crc_validate = CRC32_RESET;
    for (uint8_t i = 0; i < length-4; i++){
        crc_validate = crc32_calculate((uint32_t)input_buffer[i], crc_validate);
    }

    //validate the checksum
    if(crc_validate != union_crc32.checksum){
        return SENSOR_DATA_INVALID;
    }
    
    /*-------------------------data processing-----------------------*/
    union two_bytes{
        uint16_t value;
        uint8_t bytes[2];
    } union_two_bytes;
    
    union four_bytes{
        uint32_t value;
        uint8_t bytes[4];
    } union_four_bytes;

    //populate sensor struct
    
    //acceleration data
    union_two_bytes.bytes[0] = input_buffer[3]; //accX_MSB
    union_two_bytes.bytes[1] = input_buffer[2]; //accX_LSB  
    Global_Sensor_Data.accX = union_two_bytes.value;

    union_two_bytes.bytes[0] = input_buffer[5]; //accY_MSB
    union_two_bytes.bytes[1] = input_buffer[4]; //accZ_LSB  
    Global_Sensor_Data.accY = union_two_bytes.value;

    union_two_bytes.bytes[0] = input_buffer[7]; //accZ_MSB
    union_two_bytes.bytes[1] = input_buffer[6]; //accZ_LSB  
    Global_Sensor_Data.accZ = union_two_bytes.value;

    //gyro data
    union_two_bytes.bytes[0] = input_buffer[9]; //gyroX_MSB
    union_two_bytes.bytes[1] = input_buffer[8]; //gyroX_LSB 
    Global_Sensor_Data.gyroX = union_two_bytes.value;

    union_two_bytes.bytes[0] = input_buffer[11]; //gyroY_MSB
    union_two_bytes.bytes[1] = input_buffer[10]; //gyroY_LSB 
    Global_Sensor_Data.gyroY = union_two_bytes.value;

    union_two_bytes.bytes[0] = input_buffer[13]; //gyroZ_MSB
    union_two_bytes.bytes[1] = input_buffer[12]; //gyroZ_LSB 
    Global_Sensor_Data.gyroZ = union_two_bytes.value;

    //magneto data
    

}

