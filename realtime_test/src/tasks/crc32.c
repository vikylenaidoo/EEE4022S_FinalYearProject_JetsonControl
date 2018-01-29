
//----------------------------INCLUDES--------------------------//
#include "crc32.h"




//------------------------PUBLIC FUNCTIONS--------------------//


/** CRC32 used in stm32f4
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

//------------------------------CRC32 1952 IMPLEMEMTATION--------------------//
/**CRC32 1952 standard specified in: https://tools.ietf.org/html/rfc1952
 * code found here: https://tools.ietf.org/html/rfc1952#section-8
*/
    /* Table of CRCs of all 8-bit messages. */
    unsigned long crc_table[256];

    /* Flag: has the table been computed? Initially false. */
    int crc_table_computed = 0;

    /* Make the table for a fast CRC. */
    void make_crc_table(void){
        unsigned long c;
        int n, k;
        for (n = 0; n < 256; n++) {
            c = (unsigned long) n;
            for (k = 0; k < 8; k++) {
                if (c & 1) {
                    c = 0xedb88320L ^ (c >> 1);
                } 
                else {
                    c = c >> 1;
                }
            }
            crc_table[n] = c;
        }
        crc_table_computed = 1;
    }

    /*
        Update a running crc with the bytes buf[0..len-1] and return
    the updated crc. The crc should be initialized to zero. Pre- and
    post-conditioning (one's complement) is performed within this
    function so it shouldn't be done by the caller. Usage example:

         unsigned long crc = 0L;

         while (read_buffer(buffer, length) != EOF) {
           crc = update_crc(crc, buffer, length);
         }
         if (crc != original_crc) error();
    */


    unsigned long update_crc(unsigned long crc, unsigned char *buf, int len){
        unsigned long c = crc ^ 0xffffffffL;
        int n;

        if (!crc_table_computed)
            make_crc_table();
        for (n = 0; n < len; n++) {
            c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
        }
        return c ^ 0xffffffffL;
    }

    /* Return the CRC of the bytes buf[0..len-1]. */
    unsigned long crc32_1592_calculate(unsigned char *buf, int len){
        return update_crc(0L, buf, len);
    }