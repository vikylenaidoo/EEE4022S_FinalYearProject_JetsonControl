
#include <time.h>
#include <signal.h>

#include "tasks/uart.h"
#include "tasks/timer.h"
//#include "tasks/gpio.h"
//#include "tasks/sensor_handler.h"


//static FILE *gpio_port;
static int serial_port_xbee;
static uint8_t xbee_read_buffer[64];


static int isRunning;
static int count_drops = 0;
static int count_success = 0;