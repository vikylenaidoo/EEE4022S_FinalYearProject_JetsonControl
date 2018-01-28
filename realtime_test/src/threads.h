//--------------------INCLUDES----------------//
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "tasks/timer.h"
#include "tasks/gpio.h"
#include "tasks/uart.h"
#include "tasks/sensor_handler.h"

//-------------------VARS---------------------//
#define pin_data_rq 78

int isActive;

//gpio vars
static FILE* fp_data_rq; //will hold the file descriptor for the data request pin

//uart vars
static uint8_t uart_read_buffer[256]; //to hold chars recieved from uart
static int serial_port_data;

static uint8_t xbee_read_buffer[64]; //to hold chars recieved from xbee
static int serial_port_xbee;

static int count_drops = 0;
static int count_success = 0;

static int xb_drop = 0;

//------------------THREADS----------------//
int thread_rt_control();
int thread_xbee_telemetry();

//----------------AUX FNS------------------//
int thread_initialise();