//---------------------- INCLUDES --------------------------//
#include <stdio.h>
#include <string.h>
#include <errno.h>


//---------------------- DEFINES --------------------------//

#define SYSFS_GPIO_DIR		"/sys/class/gpio"
#define GPIO_MAX_BUF		64

typedef enum{
	GPIO_LOW = 0,
	GPIO_HIGH
} GPIO_IO_Value_Typedef;

typedef enum{
	GPIO_INPUT = 0,
	GPIO_OUTPUT
} GPIO_Direction_Typedef;

typedef enum{
	GPIOE_OK = 0,
	GPIOE_OPEN,
	GPIOE_READ,
	GPIOE_WRITE
} GPIO_Error_Typedef;

//------------------ FUNCTIONS -------------------//


//set the gpio output value to HIGH or LOW
GPIO_Error_Typedef gpio_set_value(FILE *fp, GPIO_IO_Value_Typedef val);

//initialise gpio
FILE* gpio_init(unsigned int gpio_port, GPIO_Direction_Typedef dir);

void gpio_deinit(FILE* fp, unsigned int gpio_port);