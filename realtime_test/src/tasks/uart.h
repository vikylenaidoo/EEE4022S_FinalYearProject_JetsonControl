
//---------------------- INCLUDES --------------------------//

// C library headers
#include <stdio.h>
#include <string.h>
#include <stdint.h>
//#include <stdbool.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

//---------------------- DEFINES --------------------------//

typedef enum{
    ttyTHS1,
    ttyUSB0

} UART_ttyDevice;


//---------------------- FUNCTIONS --------------------------//
int uart_init(UART_ttyDevice device, int *serial_port);
int uart_read(int *serial_port, void *uart_read_buffer, size_t size);
int uart_write(int *serial_port, char data[], size_t length);
void uart_close(int *serial_port);