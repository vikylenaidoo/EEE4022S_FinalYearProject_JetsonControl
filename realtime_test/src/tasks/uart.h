
//---------------------- INCLUDES --------------------------//

// C library headers
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

//---------------------- DEFINES --------------------------//


//---------------------- FUNCTIONS --------------------------//
void uart_init();
void uart_read(uint8_t* uart_read_buffer);
