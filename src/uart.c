
#include <xc.h>
#include <string.h>

#include "uart.h"

void uart_init(void)
{
    // SP1BRGL = 68;
    // SP1BRGL = 0x0;
    // SP1BRGH = 0x0;
    RC1STAbits.SPEN = 1;    // Enable serial port
    RC1STAbits.CREN = 1;    // Enable Continuos Receive (RX)
    TX1STAbits.TXEN = 1;    // Enable Transmit (TX)
    BAUD1CONbits.BRG16 = 1; // Set BRG to use 16 bits
    TX1STAbits.BRGH = 1;    // Select high speed baudrate
}

// Sync baudrate generator to incoming RX signal.
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

    // Make sure we didn't overflow
    // if (BAUD1CONbits.ABDOVF)
    // {
    //     while (BAUD1CONbits.RCIDL == 0)
    //     {
    //         // Wait for the fifth rising edge of the sync character.
    //         sync_trash = RC1REG;
    //     }

    //     // Clear overflow bit
    //     BAUD1CONbits.ABDOVF = 0;

    //     // Hang
    //     while (1)
    //     {
    //         CLRWDT();
    //     }
    // }

    // if (!SP1BRGL | SP1BRGH)
    // {
    //     while(1)
    //     {
    //         CLRWDT();
    //     }
    // }

    // unsigned int baudrate = SP1BRGH << 8 | SP1BRGL;

    // baudrate = baudrate << 2;

    // SP1BRGH = (baudrate << 8) & 0xF0;
    // SP1BRGL = baudrate & 0x0F;
    

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
}

void uart_write(uint8_t data)
{
    while(!TX1STAbits.TRMT)
    {
        // Transmit register full, wait for room.
    }

    TX1REG = data;
}

unsigned char
uart_read (void)
{
    while (!PIR3bits.RC1IF)
    {
        // Wait for interrupt flag to signify a byte has been received.
    }

    return RC1REG;
}


#if 0
void uart_print(char *data)
{
    while (*data) {
        uart_write((uint8_t)*data++);
    }
}

void uart_println(char *data)
{
    uint8_t newline = '\n';
    uint8_t carrige = '\r';
    while (*data) {
        uart_write((uint8_t)*data++);
        _delay(225);
    }
    uart_write(carrige);
    _delay(225);
    uart_write(newline);
        _delay(225);
}

void uart_printhex(uint8_t data)
{

    uint8_t msc = (data & 0xF0) >> 4;
    uint8_t lsc = data & 0x0F;

    if (msc <= 0x09) {
        msc += 48;  // Ascii chars 0-9
    } 
    else if (msc <= 0x0F) {
        msc  += 55; // asci chars A-F
    }
    else {
        msc = 63; // Error gets a question mark
    }
    
    if (lsc <= 0x09) {
        lsc += 48;  // Ascii chars 0-9
    } 
    else if (lsc <= 0x0F) {
        lsc  += 55; // asci chars A-F
    }
    else {
        lsc = 63; // Error gets a question mark
    }

    uart_write(msc);
    _delay(225);
    uart_write(lsc);
        _delay(225);
}

#endif