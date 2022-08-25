/**
 * Simple blocking, auto-baud syncing, 1/2-wire UART driver for PIC16(L)F1919x.
*/

#ifndef UART_H
#define	UART_H

#define UART_RX_PPS_VAL     0x14    // RX Pin selection. PPS Input pin values
#define UART_TX_PPS_REG     RC4PPS  // TX Pin selection. Output RxyPPS register

#define UART_ONE_WIRE       // Define this macro to enable 1-wire mode.

void uart_init(void);
void uart_deinit(void);
void uart_sync(void);
void uart_write(uint8_t data);
unsigned char uart_read(void);

// These are default values. They make use of EUSART1. Modifications to the
// code are needed to change these.
#define UART_RX_PPS_REG     RX1PPS  // PPS register for RX1. DO NOT EDIT!
#define UART_TX_PPS_VAL     0x0D    // PPS Output pin value of TX1. DO NOT EDIT!

#endif

// EOF //
