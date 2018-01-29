/**
 * sensor_handler.h
 * used for decoding the data received from the sensor
*/

//---------------------------INCLUDES--------------------------//
#include <stdint.h>
#include "global.h"
#include "crc32.h"

//---------------------------DEFINES--------------------------//
#define LENGTH_SENSOR_DATA  132     //how many bytes should be read from the uart 


//---------------------------PUBLIC FUNCTIONS--------------------------//


/**
* read the data from the input buffer and load it into the global structs
* there should be exactly LENGTH_SENSOR_DATA bytes of data
* sensor data starting from 3rd byte
* gnss ubx message starting from 28th byte
*/
Sensor_Error_Typedef sensor_process_data(uint8_t *in, uint8_t length);


/*print the data from global sensor struct*/
void sensor_print_data();

void print_GNSS_data();