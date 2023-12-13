/*
 * Copyright (C) 2023 Avijit Das <avijitdasxp@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <errno.h>
#include <stdint.h>
#include <windows.h>



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
    SERIAL_ERR_UNKNOWN,
    SERIAL_ERR_READ_UNKNOWN,
    SERIAL_ERR_READ_SIZE_MISMATCH,
    SERIAL_ERR_WRITE_UNKNOWN,
    SERIAL_ERR_WRITE_SIZE_MISMATCH,
}serial_port_err_t;




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

/** 
 * Objective: To set baud rate of the serial port
 * 
 * Parameters: 
 * serial_port_t* port: The serial port instance.
 * uint64_t baudRate: the Baud Rate
 * 
 * Returns: serial_port_err_t
 * SERIAL_ERR_OK: on successful setting the Baud Rate.
 * SERIAL_ERR_UNKNOWN: on unkwon reason.
*/
serial_port_err_t setBaud(serial_port_t* port, uint64_t baudRate);

/** 
 * Objective: To set read and write timeouts of the serial port
 * 
 * Parameters: 
 * serial_port_t* port: The serial port instance.
 * uint64_t readTimeout: Read timeout in ms
 * uint64_t writeTimeout: Read timeout in ms
 * 
 * Returns: serial_port_err_t
 * SERIAL_ERR_OK: on successful changing the timeout.
 * SERIAL_ERR_UNKNOWN: on unkwon reason.
*/
serial_port_err_t setTimeouts(serial_port_t* port, uint64_t readTimeout, uint64_t writeTimeout);

/** 
 * Objective: To get the system error code and 
 * print it's text format for human readability.
 * 
 * Parameters: NONE
 * 
 * Returns: NULL
*/
void printError();

#endif
