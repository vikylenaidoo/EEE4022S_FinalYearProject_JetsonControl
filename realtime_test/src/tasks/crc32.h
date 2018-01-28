#include "global.h"
#include <stdio.h>
#include <stdint.h>

//---------------------DEFINE------------------------//
#define CRC32_POLY              0x4C11DB7
#define CRC32_RESET             0xFFFFFFFF


//---------------PUBLIC FUCTIONS---------------------//

uint32_t crc32_calculate(uint32_t data, uint32_t old_crc);
