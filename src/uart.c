/**
 * Simple blocking, auto-baud capable, 1/2-wire UART driver for PIC16(L)F1919x.
*/
#include <xc.h>
#include "uart.h"

void uart_init(void)
{
    // Configure PPS module. This will set the associated TRIS bits for us.
    UART_RX_PPS_REG = UART_RX_PPS_VAL;
    UART_TX_PPS_REG = UART_TX_PPS_VAL;

    // Configure EUSART1 module.
#ifdef UART_BAUDRATE    // Setup static baudrate if defined.
    SP1BRG = (unsigned int)((32000000UL / UART_BAUDRATE) / 4) - 1;
#endif
    RC1STAbits.SPEN = 1;    // Enable serial port
    RC1STAbits.CREN = 1;    // Enable Continuos Receive (RX)
#ifndef UART_ONE_WIRE   // Only enable transmit in 2-wire mode
    TX1STAbits.TXEN = 1;    // Enable Transmit (TX)
#endif
    BAUD1CONbits.BRG16 = 1; // Set BRG to use 16 bits
    TX1STAbits.BRGH = 1;    // Select high speed baudrate
}

void
uart_sync (void)
{
    unsigned char sync_trash;

    // Start auto-baud calibration sequence
    BAUD1CONbits.ABDEN = 1;

    while(BAUD1CONbits.ABDEN)
    {
        // Wait for sync character
        CLRWDT();
    }

    // Read our trash value to clear interrupt flag
    sync_trash = RC1REG;

#if 0
    // Make sure we didn't overflow
    if (BAUD1CONbits.ABDOVF)
    {
        while (BAUD1CONbits.RCIDL == 0)
        {
            // Wait for the fifth rising edge of the sync character.
            sync_trash = RC1REG;
        }

        // Clear overflow bit
        BAUD1CONbits.ABDOVF = 0;

        // Hang
        while (1)
        {
            CLRWDT();
        }
    }
#endif

    // Subtract one from SP1BRGH:SP1BRGL register pair
    if (SP1BRGL)
    {
        // We dont have to worry about rolling over in the high byte
        sync_trash = SP1BRGL - 1;
        SP1BRGL = sync_trash;
    }
    else
    {
        // Subtract 1 from the high byte and roll over the low byte.
        sync_trash = SP1BRGH - 1;
        SP1BRGH = sync_trash;
        SP1BRGL = 0xFF;
    }
}

void
uart_deinit (void)
{
    // Reset all registers to POR value
    BAUD1CON = 0;
    RC1STA = 0;
    TX1STA = 0;
    SP1BRGL = 0;
    SP1BRGH = 0;

    // TODO: PPS registers
}

void uart_write(unsigned char data)
{
#ifdef UART_ONE_WIRE
    TX1STAbits.TXEN = 1;    // Enable Transmit (TX)
#else
    while (!TX1STAbits.TRMT)
    {
        // Wait for transmit register to be empty
    }
#endif

    // Write byte to transmit register
    TX1REG = data;

#ifdef UART_ONE_WIRE
    // These two empty while loops wait for the packet to be sent by waiting
    // for it to be received. The RCIDL bit is cleared while receiving.
    while (BAUD1CONbits.RCIDL)
    {
        // wait for start bit
        CLRWDT();
    }
    while (!BAUD1CONbits.RCIDL)
    {
        // wait to finish receiving byte sent
        CLRWDT();
    }

    data = RC1REG; // Read back sent byte

    TX1STAbits.TXEN = 0;    // Disable Transmit (TX)
#endif
}

unsigned char
uart_read (void)
{
    while (!PIR3bits.RC1IF)
    {
        // Wait for interrupt flag to signify a byte has been received.
        CLRWDT();
    }

    return RC1REG;
}

int
uart_read_bytes (int len, unsigned char * buff)
{
    for (int data = 0; data < len; data++)
    {
        while (!PIR3bits.RC1IF)
        {
            CLRWDT();
        }
        buff[data] = RC1REG;
    }
    return len;
}


// EOF //
