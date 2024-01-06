/** \file serialPort.h
 *  \author iiriis
 * Header file contains the API declarations 
 */
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



/**
 * \brief The Serial Port Structure to store the fields of the serial COMM.
 * 
 */
typedef struct {
    HANDLE handle;          /**< Handle to store the serial port as a file handle. */
    const char *name;       /**< Name of the serial COM port. */
    boolean isOpen;         /**< Boolean value to indicate the port open status. */
    uint64_t baud;          /**< Baud rate of the serial COM port. */
    uint32_t readTimeout;   /**< Read timeout in ms. */
    uint32_t writeTimeout;  /**< Write timeout in ms. */
}serial_port_t;

/**
 * \brief Error enum to store the error types.
 *
 * This enum is used to store the error types that can be returned by the serial port functions.
 * The enum values correspond to different types of errors that may occur during serial port operations.
 * The meaning of each value is described below.
 */
typedef enum {
    SERIAL_ERR_OK,                 /**< No error occurred. */
    SERIAL_ERR_OPEN,               /**< Error occurred while opening the serial port. */
    SERIAL_ERR_CLOSE,              /**< Error occurred while closing the serial port. */
    SERIAL_ERR_UNKNOWN,            /**< Unknown error occurred. */
    SERIAL_ERR_READ_UNKNOWN,       /**< Unknown error occurred during read operation. */
    SERIAL_ERR_READ_SIZE_MISMATCH, /**< The number of bytes read doesn't match the expected size. */
    SERIAL_ERR_WRITE_UNKNOWN,      /**< Unknown error occurred during write operation. */
    SERIAL_ERR_WRITE_SIZE_MISMATCH /**< The number of bytes written doesn't match the buffer size. */
}serial_port_err_t;


/**
 * @brief Open the serial port and initialize the serial port handle.
 *
 * @param port Pointer to the serial port structure.
 * @param name String containing the name of the serial port.
 * @param baud Baudrate of the serial port.
 * @param readTimeout Timeout in ms before read action returns.
 * @param writeTimeout Timeout in ms before write action returns.
 *
 * @returns
 *  `SERIAL_ERR_OK`: on successful opening the serial port. \n
 *  `SERIAL_ERR_OPEN`: could not open serial port. \n
 */
serial_port_err_t serialPortOpen(serial_port_t* port, const char* name, uint64_t baud, uint32_t readTimeout, uint32_t writeTimeout);

/** 
 * @brief To close the serial port instance and print 
 * any error if occured.
 * 
 * @param port The serial port structure to close.
 * 
 * @returns 
 * `SERIAL_ERR_OK`: on successfully closing the serial port. \n
 * `SERIAL_ERR_CLOSE`: error occured while closing the serial port. \n
*/
serial_port_err_t serialPortClose(serial_port_t* port);

/** 
 * @brief To read from the serial port
 *  
 * @param port: The serial port instance.
 * @param buf: a buffer to store the read data.
 * @param size: nos of expected bytes to read.
 * 
 * @returns
 * `SERIAL_ERR_OK`: on successfully reading the expected nos of bytes. \n
 * `SERIAL_ERR_READ_UNKNOWN`: on unkwon reason. \n
 * `SERIAL_ERR_READ_SIZE_MISMATCH`: nos of bytes read != expected size. \n
*/
serial_port_err_t serialPortRead(serial_port_t* port, uint8_t *buf, uint64_t size);

/** 
 * @brief To write to the serial port
 * 
 * @param port: The pointer to serial port instance.
 * @param buf: a buffer containing the data to write.
 * @param size: size of the buffer.
 * 
 * @returns
 * `SERIAL_ERR_OK`: on successfully writing the full buffer. \n
 * `SERIAL_ERR_WRITE_UNKNOWN`: on unkwon reason. \n
 * `SERIAL_ERR_WRITE_SIZE_MISMATCH`: nos of bytes written != buffer size. \n
*/
serial_port_err_t serialPortWrite(serial_port_t* port, uint8_t *buf, uint64_t size);

/** 
 * @brief To set baud rate of the serial port
 * 
 * @param port: The pointer to the serial port instance.
 * @param baudRate: the Baud Rate
 * 
 * @returns
 * `SERIAL_ERR_OK`: on successful setting the Baud Rate. \n
 * `SERIAL_ERR_UNKNOWN`: on unkwon reason. \n
*/
serial_port_err_t setBaud(serial_port_t* port, uint64_t baudRate);

/** 
 * @brief To set read and write timeouts of the serial port
 * 
 * Parameters: 
 * @param port: The pointer to the serial port instance.
 * @param readTimeout: Read timeout in ms
 * @param writeTimeout: Read timeout in ms
 * 
 * @returns
 * `SERIAL_ERR_OK`: on successful changing the timeout. \n
 * `SERIAL_ERR_UNKNOWN`: on unkwon reason. \n
*/
serial_port_err_t setTimeouts(serial_port_t* port, uint64_t readTimeout, uint64_t writeTimeout);

/** 
 * @brief To get the system error code and 
 * print it's text format for human readability.
 * 
 * @param NONE
 * 
 * @returns NULL
*/
void printError();

#endif
