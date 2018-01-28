//--------------------INCLUDES----------------//
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "tasks/timer.h"
#include "tasks/gpio.h"
//#include "tasks/uart.h"
#include "tasks/sensor_handler.h"
#include "tasks/pololu_servo.h"

//-------------------VARS---------------------//
#define pin_data_rq 78

int isRunning;
int isAlive; 
int controlMode; //0=manual; 1=control;

//global time counter
static uint32_t time_counter = 0; //holds 4ms increments

//logging vars
static FILE* fp_data_log; //fp to log all data+timestamp

//gpio vars
static FILE* fp_data_rq; //will hold the file descriptor for the data request pin

//uart vars
static int serial_port_servo;

static uint8_t uart_read_buffer[256]; //to hold chars recieved from uart
static int serial_port_data;

static uint8_t xbee_read_buffer[32]; //to hold chars recieved from xbee
static int serial_port_xbee;

static int count_drops = 0;
static int count_success = 0;

static int xb_drop = 0;

//------------------THREADS----------------//
int thread_rt_control();
int thread_xbee_telemetry();

//----------------AUX FNS------------------//
int thread_initialise();