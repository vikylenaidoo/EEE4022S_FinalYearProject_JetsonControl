
#include <stdio.h>
#include <stdint.h>

//---------------------DEFINE------------------------//
#define CRC32_POLY              0x4C11DB7
#define CRC32_RESET             0xFFFFFFFF

typedef enum{
    CRC32_OK=0,
    CRC32_FAIL

} CRC32_Error_Typedef;


//---------------PUBLIC FUCTIONS---------------------//

uint32_t crc32_calculate(uint32_t data, uint32_t old_crc);

//----------------------crc32 1592 functions--------------// 
void make_crc_table(void);
unsigned long update_crc(unsigned long crc, unsigned char *buf, int len);
unsigned long crc32_1592_calculate(unsigned char *buf, int len);