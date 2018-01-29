/**
 * sensor_handler.h
 * used for decoding the data received from the sensor
*/

#include "sensor_handler.h"

//---------------------------DEFINES--------------------------//

//---------------------------STATIC FUNCTIONS--------------------------//


//---------------------------PUBLIC FUNCTIONS--------------------------//


Sensor_Error_Typedef sensor_process_data(uint8_t input_buffer[], uint8_t length){
    
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
    
    //use big endian
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
    union_two_bytes.bytes[0] = input_buffer[2]; //accX_LSB
    union_two_bytes.bytes[1] = input_buffer[3]; //accX_MSB  
    Global_Sensor_Data.accX = union_two_bytes.value;

    union_two_bytes.bytes[0] = input_buffer[4]; //accY_LSB
    union_two_bytes.bytes[1] = input_buffer[5]; //accZ_MSB  
    Global_Sensor_Data.accY = union_two_bytes.value;

    union_two_bytes.bytes[0] = input_buffer[6]; //accZ_LSB
    union_two_bytes.bytes[1] = input_buffer[7]; //accZ_MSB  
    Global_Sensor_Data.accZ = union_two_bytes.value;

    //gyro data
    union_two_bytes.bytes[0] = input_buffer[8]; //gyroX_LSB
    union_two_bytes.bytes[1] = input_buffer[9]; //gyroX_MSB 
    Global_Sensor_Data.gyroX = union_two_bytes.value;

    union_two_bytes.bytes[0] = input_buffer[10]; //gyroY_LSB
    union_two_bytes.bytes[1] = input_buffer[11]; //gyroY_MSB 
    Global_Sensor_Data.gyroY = union_two_bytes.value;

    union_two_bytes.bytes[0] = input_buffer[12]; //gyroZ_LSB
    union_two_bytes.bytes[1] = input_buffer[13]; //gyroZ_MSB 
    Global_Sensor_Data.gyroZ = union_two_bytes.value;

    //magneto data
    union_two_bytes.bytes[0] = input_buffer[14]; //magX_LSB    
    union_two_bytes.bytes[1] = input_buffer[15]; //magX_MSB
    Global_Sensor_Data.magX = union_two_bytes.value;

    union_two_bytes.bytes[0] = input_buffer[16]; //magY_LSB    
    union_two_bytes.bytes[1] = input_buffer[17]; //magY_MSB
    Global_Sensor_Data.magY = union_two_bytes.value;

    union_two_bytes.bytes[0] = input_buffer[18]; //magZ_LSB    
    union_two_bytes.bytes[1] = input_buffer[19]; //magZ_MSB
    Global_Sensor_Data.magZ = union_two_bytes.value;

    union_two_bytes.bytes[0] = input_buffer[20]; //magHall_LSB    
    union_two_bytes.bytes[1] = input_buffer[21]; //magHall_MSB
    Global_Sensor_Data.magHall = union_two_bytes.value;

    //baro data
    union_four_bytes.bytes[3] = 0;
    union_four_bytes.bytes[2] = input_buffer[22]; //baroPress_MSB
    union_four_bytes.bytes[1] = input_buffer[23]; //baroPress_LSB
    union_four_bytes.bytes[0] = input_buffer[24] << 4; //baroPress_XLSB
    Global_Sensor_Data.baroPress = union_four_bytes.value >> 4;
    //printf("BaroPress: \t %x %x %x %x \n", union_four_bytes.bytes[3], union_four_bytes.bytes[2], union_four_bytes.bytes[1], union_four_bytes.bytes[0]);

    union_four_bytes.bytes[3] = 0;
    union_four_bytes.bytes[2] = input_buffer[25]; //baroTemp_MSB
    union_four_bytes.bytes[1] = input_buffer[26]; //baroTemp_LSB
    union_four_bytes.bytes[0] = input_buffer[27] << 4; //baroTemp_XLSB
    Global_Sensor_Data.baroTemp = union_four_bytes.value >> 4; 

    //populate GNSS data

    //int i = 34;
    int index = 0;
    int gnss_length = sizeof(Global_GNSS_Data)/sizeof(uint8_t);
    //printf("gnss length %d\n", gnss_length);

    while(index < gnss_length){
        ((uint8_t*)(&Global_GNSS_Data))[index] = input_buffer[index+34];
        index++;

    }
    
    //sensor_print_data();
    print_GNSS_data();

    return SENSOR_OK;
}



void sensor_print_data(){
    printf("-------------------- printing global sensor struct -------------------\n");
    printf("accX: \t\t\t %d \t\t\t %x \n", Global_Sensor_Data.accX, Global_Sensor_Data.accX);
    printf("accY: \t\t\t %d \t\t\t %x \n", Global_Sensor_Data.accY, Global_Sensor_Data.accY);
    printf("accZ: \t\t\t %d \t\t\t %x \n", Global_Sensor_Data.accZ, Global_Sensor_Data.accZ);

    printf("gyroX: \t\t\t %d \t\t\t %x \n", Global_Sensor_Data.gyroX, Global_Sensor_Data.gyroX);
    printf("gyroY: \t\t\t %d \t\t\t %x \n", Global_Sensor_Data.gyroY, Global_Sensor_Data.gyroY);
    printf("gyroZ: \t\t\t %d \t\t\t %x \n", Global_Sensor_Data.gyroZ, Global_Sensor_Data.gyroZ);

    
    printf("magX: \t\t\t %d \t\t\t %x \n", Global_Sensor_Data.magX, Global_Sensor_Data.magX);
    printf("magY: \t\t\t %d \t\t\t %x \n", Global_Sensor_Data.magY, Global_Sensor_Data.magY);
    printf("magZ: \t\t\t %d \t\t\t %x \n", Global_Sensor_Data.magZ, Global_Sensor_Data.magZ);
    printf("magHall: \t\t\t %d \t\t\t %x \n", Global_Sensor_Data.magHall, Global_Sensor_Data.magHall);

    printf("baroPress: \t\t\t %d \t\t\t %x \n", Global_Sensor_Data.baroPress, Global_Sensor_Data.baroPress);
    printf("baroTemp: \t\t\t %d \t\t\t %x \n", Global_Sensor_Data.baroTemp, Global_Sensor_Data.baroTemp);

    printf("----------------------- end printing --------------------------------\n");
}

void print_GNSS_data(){
    printf("hour:min:sec:nano \t %d : %d : %d : %d \n", Global_GNSS_Data.GNSS_hour,Global_GNSS_Data.GNSS_min, Global_GNSS_Data.GNSS_sec, Global_GNSS_Data.GNSS_iTow);

}
