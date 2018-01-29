
//----------------------------INCLUDES--------------------------//
#include "crc32.h"




//------------------------PUBLIC FUNCTIONS--------------------//


/**
 * Create a CRC32 checksum from 32bit input data
 * first calc use CRC32_RESET for old_crc
 * if more than 1 word needs to be calculate, use old_crc from previous calc
*/
uint32_t crc32_calculate(uint32_t data, uint32_t old_crc){
    
    //initialise
    uint32_t crc = old_crc^data;
    uint8_t i = 0;

    //calculate
    while(i < 32){
        if(crc & 0x80000000){
            crc = (crc << 1)^CRC32_POLY;
        }
        else{
            crc = (crc << 1);
        }
        i++;
    }

    return crc;

}