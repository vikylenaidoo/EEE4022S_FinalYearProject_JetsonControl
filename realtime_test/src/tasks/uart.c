/**
 * Author:          Vikyle Naidoo
 * Date:            23/09/2020
 * Description:     library for configuring and using uart serial comms
 * 
 * https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
*/


//---------------------- INCLUDES --------------------------//
#include "uart.h"

//---------------------- DEFINES --------------------------//


//---------------------- PUBLIC FUNCTIONS --------------------------//

/**
 * initialise the uart port
 * input: tty serial device to be configured
 * default config:  stop bit, no parity, bd115200
 * returns 0 if success, otherwise errno 
 */
int uart_init(UART_ttyDevice device, int *serial_port){
    //printf("---------starting uart open-------------\n");
    //open the serial port
    switch (device){
    case ttyTHS1:
        *serial_port = open("/dev/ttyTHS1", O_RDWR);
        break;
    case ttyUSB0:
        *serial_port = open("/dev/ttyUSB0", O_RDWR);
        break;
    default:
        *serial_port = -1;
        break;
    }
    if (*serial_port < 0) { // Check for errors
        printf("error open");
        return errno;
    }

    //printf("---------starting uart config-------------\n");
    //configuration setup
    struct termios tty;
    if(tcgetattr(*serial_port, &tty) != 0) { // Read in existing settings, and handle any error
        printf("error config");
        return errno;
    }

    //printf("---------starting uart control-------------\n");
    //control modes
    tty.c_cflag &= ~PARENB; //disable parity
    //tty.c_cflag &= ~CSTOPB; //single stop bit
    tty.c_cflag |= CSTOPB; //2 stop bit
    tty.c_cflag |= CS8; // 8 bits per byte
    tty.c_cflag &= ~CRTSCTS; //disable RTS/CTS hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    //local modes
    tty.c_lflag &= ~ICANON; //disable canonical mode
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

    //input modes
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    //output modes
    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    //timeout settings: vmin=0, vtime=0 ==> no blocking, return immediaately what is available
    tty.c_cc[VMIN] = 0; //amount of chars to read
    tty.c_cc[VTIME] = 0; //timeout of read

    //baud rate: 460800
    cfsetispeed(&tty, B460800); //input baud
    cfsetospeed(&tty, B460800); //output baud

    if(device == ttyUSB0){
        tty.c_cflag &= ~CSTOPB; //single stop bit
        //tty.c_cc[VMIN] = 10; //message size is 10 bytes
        //tty.c_cc[VTIME] = 1; //timeout (ds)

        cfsetispeed(&tty, B9600); //input baud
        cfsetospeed(&tty, B9600); //output baud
    }

    //printf("---------starting uart save-------------\n");
    // Save tty settings, and checking for error
    if (tcsetattr(*serial_port, TCSANOW, &tty) != 0) {
        return errno;
    }

    return 0;
}

/**
 * read bytes from serial and put into buffer
 * input:   int *serial_port            the fd to the serial port to read from
 *          uint8_t *uart_read_buffer   buffer to put read bytes
 * return:  number read, -1 for errors or 0 for EOF.
*/
int uart_read(int *serial_port, void *uart_read_buffer, size_t size){

    return read(*serial_port, uart_read_buffer, size);

}

/**
 * write bytes to uart
*/
int uart_write(int *serial_port, void *data, size_t length){
    return write(*serial_port, data, length);
}

/*close the serial port*/
void uart_close(int *serial_port){
    close(*serial_port);
}

