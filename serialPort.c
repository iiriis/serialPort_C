
#include <stdio.h>
#include "serialPort.h"

void printError()
{
    /* Create a buffer to store the error message */
    char buf[MAX_ERR_CODE_LEN];

    /* Use the FormatMessage API to get the error message from the system */
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK, NULL, GetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), buf, MAX_ERR_CODE_LEN, NULL);

    /* Print the error message in red color and give beep tone*/
    printf("\x1b[31m%s\x1b[0m\a\n", buf);
}


serial_port_err_t setTimeouts(serial_port_t* port, uint64_t readTimeout, uint64_t writeTimeout)
{
    /* create a COMMTIMEOUTS structure and set the timeout values */
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = 0;           /* No timeout between subsequent reads */
    timeouts.ReadTotalTimeoutConstant = readTimeout;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = writeTimeout;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    /* store the timeout values in the serial port handle */
    port->readTimeout = readTimeout;
    port->writeTimeout = writeTimeout;

    /* set the timeouts and check for error */
    if (!SetCommTimeouts(port->handle, &timeouts)) 
        return SERIAL_ERR_UNKNOWN;
    
    /* return OK */
    return SERIAL_ERR_OK;
}


serial_port_err_t setBaud(serial_port_t* port, uint64_t baudRate)
{
    /* create a DCB structure and set the Baudrate */
    DCB dcb = {0};

    /* set the size of DCB length to the size of the structure itself */
    dcb.DCBlength = sizeof(DCB);
    
    /* get the current DCB state */
    if (!GetCommState(port->handle, &dcb))
        return SERIAL_ERR_UNKNOWN;

    dcb.BaudRate = baudRate;
    port->baud = baudRate;

    /* set the new DCB values for the serial port */
    if (!SetCommState(port->handle, &dcb))
        return SERIAL_ERR_UNKNOWN;

    /* return OK */
    return SERIAL_ERR_OK;
}


serial_port_err_t serialPortOpen(serial_port_t* port, const char* name, uint64_t baud, uint32_t readTimeout, uint32_t writeTimeout) 
{

    /* Cosmetic colorisation of the terminal output by enabling Virtual Terminal */
    DWORD currentConsoleMode;
    /* get the current console attributes and OR with virtual terminal flag */
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &currentConsoleMode);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), currentConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING );


    /* initialise the port structure with the arguments */
    port->name = name;
    port->baud = baud;
    port->isOpen = FALSE;
    port->readTimeout = readTimeout;
    port->writeTimeout = writeTimeout;

    /* set the baud rate and the timeouts */
    setBaud(port, baud);
    setTimeouts(port, readTimeout, writeTimeout);
    
    /* open the serial port by opening it as a file with the following attributes */
    port->handle = CreateFileA(port->name, FILE_RW_MODE, FILE_NO_SHARED_ACCESS, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    /* check whether the port handle is invalid */
    if(port->handle == INVALID_HANDLE_VALUE){
        /* print the error code and return Err */
        printError();
        return SERIAL_ERR_OPEN;
    }

    /* set the port is open to TRUE */
    port->isOpen = TRUE;

    /* return OK */
    return SERIAL_ERR_OK;
}


serial_port_err_t serialPortClose(serial_port_t* port)
{
    /* Close the port handle and set the isOpen to FALSE upon success*/
    if (CloseHandle(port->handle))
    {
        port->isOpen = FALSE;
        /* return OK */
        return SERIAL_ERR_OK;
    }

    /* if close was unsuccessful, print the error code */
    printError();

    /* return error */
    return SERIAL_ERR_CLOSE;
}


serial_port_err_t serialPortRead(serial_port_t* port, uint8_t *buf, uint64_t size)
{
    /* to store the actual bytes read */
    uint64_t bytesRead;

    /* read from the serial port and check if it's a successful read */
    if(ReadFile(port->handle, buf, size, (LPDWORD)&bytesRead, NULL) != TRUE)
    {
        /* upon read failure print and return error */
        printError();
        return SERIAL_ERR_READ_UNKNOWN;
    }

    /* if the actual bytes read and the size requested are not same; return error */
    if(bytesRead != size)
        return SERIAL_ERR_READ_SIZE_MISMATCH;

    /* return OK */
    return SERIAL_ERR_OK;
}


serial_port_err_t serialPortWrite(serial_port_t* port, uint8_t *buf, uint64_t size)
{
    /* to store the actual bytes written */
    uint64_t bytesWrite;

    /* write to the serial port and check if it's a successful write */
    if(WriteFile(port->handle, buf, size, (LPDWORD)&bytesWrite, NULL) != TRUE)
    {
        /* upon write failure print and return error */
        printError();
        return SERIAL_ERR_WRITE_UNKNOWN;
    }

    /* if the actual bytes written and the size of buffer are not same; return error */
    if(bytesWrite != size)
        return SERIAL_ERR_WRITE_SIZE_MISMATCH;

    /* return OK */
    return SERIAL_ERR_OK;
}


int main()
{

    serial_port_t port;
    uint8_t buf[5];
    uint64_t size=1;

    serialPortOpen(&port, "COM7", 115200, 1000, 1000);

    char *subPwr = "$PAIR002*38\r\n";
 
    serialPortWrite(&port, subPwr, strlen(subPwr));

    while(1){
    serialPortRead(&port, buf, 1);
    putchar(buf[0]);
    }

    serialPortClose(&port);

    return 0;
}

