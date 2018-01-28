//---------------------- INCLUDES ------------------------//

#include "gpio.h"

//---------------------- DEFINES --------------------------//


//---------------------- STATIC FUNCTIONS --------------------------//
static GPIO_Error_Typedef gpio_export(unsigned int gpio_port){
	FILE *fp = fopen(SYSFS_GPIO_DIR "/export", "w");
	fprintf(fp, "%d", gpio_port);
	fclose(fp);
	
	return GPIOE_OK;
}

static GPIO_Error_Typedef gpio_unexport(unsigned int gpio_port){
	FILE *fp = fopen(SYSFS_GPIO_DIR "/unexport", "w");
	fprintf(fp, "%d", gpio_port);
	fclose(fp);
	
	return GPIOE_OK;
}

static GPIO_Error_Typedef gpio_set_direction(unsigned int gpio_port, GPIO_Direction_Typedef dir){
	char filename[256];
	sprintf(filename, SYSFS_GPIO_DIR "/gpio%d/direction", gpio_port);
	FILE* fp = fopen(filename, "w");
	
	if(!fp){
			return GPIOE_OPEN;
	}
	
	if(dir == GPIO_INPUT){
		fprintf(fp, "in");
	}
	else{
		fprintf(fp, "out");
	}
	
	fclose(fp);
	
	return GPIOE_OK;
}


//return the file pointer of thespecified gpio port
static FILE* gpio_open_file(unsigned int gpio_port){
	char filename [256];
	sprintf(filename, SYSFS_GPIO_DIR "/gpio%d/value", gpio_port);
	FILE* fp = fopen(filename, "w");
	
	return fp;
}

//---------------------- PUBLIC FUNCTIONS --------------------------//

//set the gpio output value to HIGH or LOW
GPIO_Error_Typedef gpio_set_value(FILE *fp, GPIO_IO_Value_Typedef val){
	fprintf(fp, "%d\n", val);
	if(fflush(fp)){
		return GPIOE_WRITE;
	}
	return GPIOE_OK;
}

//initialise gpio
FILE* gpio_init(unsigned int gpio_port, GPIO_Direction_Typedef dir){
	
	//export pin to gpio directory
	gpio_export(gpio_port);
	
	//set pin as input/output
	gpio_set_direction(gpio_port, dir);
	
	//open file pointer
	FILE* fp = gpio_open_file(gpio_port);
	return fp;
}

void gpio_deinit(FILE* fp, unsigned int gpio_port){
	gpio_set_value(fp, GPIO_LOW);
	gpio_unexport(gpio_port);
    fclose(fp);
}