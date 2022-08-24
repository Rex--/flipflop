#ifndef UART_H
#define	UART_H

#include <stdint.h>

void uart_init(void);
void uart_deinit(void);
void uart_sync(void);
void uart_write(uint8_t data);
unsigned char uart_read(void);

// void uart_print(char *data);
// void uart_println(char *data);
// void uart_printhex(uint8_t data);

#endif