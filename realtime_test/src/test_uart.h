
#include <time.h>
#include <signal.h>

#include "tasks/uart.h"
#include "tasks/gpio.h"
#include "tasks/sensor_handler.h"


static FILE *gpio_port;
static int serial_port;
static uint8_t uart_read_buffer[256];


static int isActive;
static int count_drops = 0;
static int count_success = 0;