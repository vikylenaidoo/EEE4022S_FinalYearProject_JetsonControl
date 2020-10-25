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

#define RT_PERIOD   10   //change this to choose control loop frequency (ms)

//status flags
int isRunning;  //determines if the entire program is running. if isRunning==0, program exits
int isAlive;    //determines the status of the cars motion
int controlMode; //0=manual; 1=control;


//global time counter
static uint32_t time_counter = 0; //holds 4ms increments

//logging vars
static FILE* fp_data_log; //fp to log all data+timestamp

//gpio vars
static FILE* fp_data_rq; //will hold the file descriptor for the data request pin

//uart vars
static int serial_port_servo; //comms with the pololu servo driver

static uint8_t uart_read_buffer[256]; //to hold chars recieved from uart
static int serial_port_data; //sensor board comms

static uint8_t xbee_read_buffer[32]; //to hold chars recieved from xbee
static int serial_port_xbee;    //telemetry comms

static int count_drops = 0;
static int count_success = 0;

static int xb_drop = 0;

//------------------THREADS----------------//
int thread_rt_control();
int thread_xbee_telemetry();

//----------------AUX FNS------------------//
int thread_initialise();