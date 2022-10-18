/**
 * Simple blocking, auto-baud syncing, 1/2-wire UART driver for PIC16(L)F1919x.
*/

#ifndef UART_H
#define	UART_H

/**
 * Baudrate of UART connection.
 * Typical baudrate are not supported and this driver makes no effort
 * to determine which one is meant. We can, however, get close enough for
 * several:
 *  [Typ] - [Act]  [Clock Speed]
 *   9600 - 9615   (1, 4, 8 MHz)
 *   9600 - 9604   (32 MHz)
 * 
 *  19200 - 19230  (1, 4, 8 MHz)
 *  19200 - 19180  (32 MHz)
 * 
 * 115200 - 115900 (32 MHz)
 * NOTE: Undef if using auto-baud. Saves 4 words of flash.
*/
#undef UART_BAUDRATE

// #define UART_RX_PPS_VAL     0x12    // RX Pin selection. PPS Input pin values
// #define UART_TX_PPS_REG     RC2PPS  // TX Pin selection. Output RxyPPS register

#define UART_ONE_WIRE       // Define this macro to enable 1-wire mode.

/**
 * Initialize UART connection.
 * This sets up the EUSART1 module on the pins specified in the header at the
 * configured baudrate, if set. If auto-baud is wanted then a call to
 * uart_sync() must follow before using the connection.
*/
void uart_init(void);

/**
 * De-initialize UART connection.
 * This sets all registers back to their POR defaults.
*/
void uart_deinit(void);

/**
 * Sync the UART connection to an incoming packet.
 * This requires an ascii 'U' to be received over the UART connection to
 * automatically calculate the baudrate of the incoming signal. The UART
 * connection must first be initialized.
*/
void uart_sync(void);

/**
 * Write a byte of data to the UART.
 *  In 2-wire mode, if no data is held in the transmit buffer, this will return
 * immediately. However if the transmit buffer is full, this will block for max
 * the amount of time it takes to transmit a single packet.
 *  In 1-wire mode, this will always block for the amount of time it takes to
 * transmit a single packet.
 * 
 * @param[in]   data    Byte of data.
*/
void uart_write(unsigned char data);

/**
 * Read a byte of data from the UART.
 * Returns the last received byte of data. If no data has been received, or if
 * all data has been read, it will block and wait for an incoming byte.
 * 
 * To check if data is available: PIR3bits.RC1IF
 * 
 * @returns     Byte of data.
*/
unsigned char uart_read(void);

/**
 * Read n-bytes from the UART into a buffer.
 * 
 * @param[in]   len     Number of bytes to read.
 * @param[out]  buff    Buffer to read into.
 * 
 * @returns     Number of bytes read, -1 if error.
*/
int uart_read_bytes(int len, unsigned char * buff);

// These are default values. They make use of EUSART1. Modifications to the
// code are needed to change these.
#define UART_RX_PPS_REG     RX1PPS  // PPS register for RX1. DO NOT EDIT!
#define UART_TX_PPS_VAL     0x0D    // PPS Output pin value of TX1. DO NOT EDIT!

#endif

// EOF //
