
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <windows.h>


#define FILE_NO_SHARED_ACCESS   0
#define FILE_RW_MODE            (FILE_GENERIC_READ | FILE_GENERIC_WRITE)
#define MAX_ERR_CODE_LEN        64u

/* The Serial Port Structure to store the fields of the serial COMM */
typedef struct {
    HANDLE handle;          /* Handle to store the serial port as a file handle */
    const char *name;       /* name of the serial COM port */
    boolean isOpen;         /* Boolean value to indicate the port open status */
    uint64_t baud;          /* Baud rate of the serial COM port */
    uint32_t readTimeout;   /* read timeout in ms */
    uint32_t writeTimeout;  /* write timeout in ms */
}serial_port_t;

/* error enum to store the error types */
typedef enum {
    SERIAL_ERR_OK,
    SERIAL_ERR_OPEN,
    SERIAL_ERR_CLOSE,
    SERIAL_ERR_READ_UNKNOWN,
    SERIAL_ERR_READ_SIZE_MISMATCH,
    SERIAL_ERR_WRITE_UNKNOWN,
    SERIAL_ERR_WRITE_SIZE_MISMATCH,
}serial_port_err_t;


/** 
 * Objective: To get the system error code and 
 * print it's text format for human readability.
 * 
 * Parameters: NONE
 * 
 * Returns: NULL
*/
void printError();

void printError()
{
    /* Create a buffer to store the error message */
    char buf[MAX_ERR_CODE_LEN];

    /* Use the FormatMessage API to get the error message from the system */
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK, NULL, GetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), buf, MAX_ERR_CODE_LEN, NULL);

    /* Print the error message in red color and give beep tone*/
    printf("\x1b[31m%s\x1b[0m\a\n", buf);
}

/** 
 * Objective: To open the serial port and initialise the 
 * the serial port handle.
 * 
 * Parameters: 
 * serial_port_t* port: Pointer to the serial port structure.
 * const char* name: String containing the name of the serial port.
 * uint64_t baud: Baudrate of the serial port.
 * uint32_t readTimeout: timeout in ms before read action returns.
 * uint32_t writeTimeout: timeout in ms before write action returns.
 *  
 * Returns: serial_port_err_t
 * SERIAL_ERR_OK: on successful poening the serial port.
 * SERIAL_ERR_OPEN: could not open serial port.
*/
serial_port_err_t serialPortOpen(serial_port_t* port, const char* name, uint64_t baud, uint32_t readTimeout, uint32_t writeTimeout);

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

/** 
 * Objective: To close the serial port instance and print 
 * any error if occured.
 * 
 * Parameters: 
 * serial_port_t* port: The serial port structure to close.
 * 
 * Returns: serial_port_err_t
 * SERIAL_ERR_OK: on successfully closing the serial port.
 * SERIAL_ERR_CLOSE: error occured while closing the serial port.
*/
serial_port_err_t serialPortClose(serial_port_t* port);

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

/** 
 * Objective: To read from the serial port
 * 
 * Parameters: 
 * serial_port_t* port: The serial port instance.
 * uint8_t *buf: a buffer to store the read data.
 * uint64_t size: nos of expected bytes to read.
 * 
 * Returns: serial_port_err_t
 * SERIAL_ERR_OK: on successfully reading the expected nos of bytes.
 * SERIAL_ERR_READ_UNKNOWN: on unkwon reason.
 * SERIAL_ERR_READ_SIZE_MISMATCH: nos of bytes read != expected size.
*/
serial_port_err_t serialPortRead(serial_port_t* port, uint8_t *buf, uint64_t size);

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


/** 
 * Objective: To write to the serial port
 * 
 * Parameters: 
 * serial_port_t* port: The serial port instance.
 * uint8_t *buf: a buffer containing the data to write.
 * uint64_t size: size of the buffer.
 * 
 * Returns: serial_port_err_t
 * SERIAL_ERR_OK: on successfully writing the full buffer.
 * SERIAL_ERR_WRITE_UNKNOWN: on unkwon reason.
 * SERIAL_ERR_WRITE_SIZE_MISMATCH: nos of bytes written != buffer size.
*/
serial_port_err_t serialPortWrite(serial_port_t* port, uint8_t *buf, uint64_t size);

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

