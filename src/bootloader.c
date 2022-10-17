/**
 * Main boot loader application. 
*/

#include <xc.h>

#include "bootloader.h"
#include "uart.h"
#include "nvm.h"

// Buffer that holds a commmand (1 byte) + its arguments (max 4 bytes).
static unsigned char command_buff[4];

// This is based on the amount of ram available. Theres 2KB in a PIC16LF19196
// which is what is being used to test.
#define MAX_COMMAND_DATA 2000
// Buffer that holds command data.
unsigned char command_data[MAX_COMMAND_DATA];

// Handle a boot loader command
unsigned char bootloader_command(void);

void
bootloader_start (void)
{
    // Init UART driver
    uart_init();

    while(command_buff[0] != 'U')
    {
        // Sync to baudrate
        uart_sync();

        // Receive second sync U
        command_buff[0] = uart_read();
    }

    // Transmit Hello
    uart_write('K');

    // Wait for command
    while (1)
    {
        // If byte has been received, assume its a command.
        command_buff[0] = uart_read();
        if (bootloader_command())
        {
            break;
        }
    }

    uart_deinit();
}


#define MAX_ADDR 0xF0FF
/**
 * Handle a boot loader command.
 * 
 * The command is assumed to be in the command_buff already. The arguments may
 * or may not have been sent yet. This function will block and collect the
 * necessary arguments to the command, if needed. It will store these in the
 * command buffer.
 * 
 * Returns 1 if the command doesn't exist,
 * 0 on success.
*/
unsigned char
bootloader_command (void)
{
    unsigned char ret = 0;
    unsigned int address;
    unsigned int data;
    int length;

    switch (command_buff[0])
    {

    case 'U':   // Reset device
        RESET();
    break;

    case 'X':   // Start user app
        ret = 1;
    break;

    case 'R':   // Read words
        // 14-bit Address argument (2 bytes) & Length argument (2 bytes)
        uart_read_bytes(4, command_buff);

        address = (unsigned int)((command_buff[0] << 8) | command_buff[1]);
        length = ((command_buff[2] << 8) | command_buff[3]);

        if ((address > MAX_ADDR) || (length > MAX_COMMAND_DATA))
        {
            uart_write('E');
        }
        else
        {
            // Returns # of bytes read - takes # of words to read
            length = nvm_read_words(address, (length/2), command_data);
            for (int resp = 0; resp < length; resp++)
            {
                uart_write(command_data[resp]);
            }
        }
    break;

    case 'W':   // Write words
        // Address argument (2 bytes) & Length argument (2 bytes)
        uart_read_bytes(4, command_buff);

        // Read <Length> bytes
        length = ((command_buff[2] << 8) | (command_buff[3]));
        uart_read_bytes(length, command_data);

        // Read 2 byte checksum
        // unsigned char checksum_buff[2];
        // uart_read_bytes(2, checksum_buff);

        // We now have all the data. Next steps:

        // 1. Verify checksum. Don't waste anymore time with bad data.
            // a. Return 'E' on failed checksum

        // 2. Write data in memory to flash.
            // a. Determine start row address.
                // 1a. We could figure this out programmatically somehow
                // 2a. We could assume the address is the start row address
            // b. Pad data into rows if necessary.
            // c. Write row.
            // d. Verify row.
                // 1d. Retry write
                // 2d. Return 'E'
            // e. Advance to next row in data block.
            // f. Return 'K'
        
        address = (unsigned int)((command_buff[0] << 8) | command_buff[1]);

        // Write to CONFIG/EEPROM if address is out of PFM bounds
        if (address > 0x7FFF)
        {
            // Only accept write commands to config/eeprom that are 1 word long
            if (length == 2)
            {
                data = (unsigned int)((command_data[0] << 8) | command_data[1]);
                // write config or eeprom
                nvm_write(address, data);
                uart_write('K');
            }
            else
            {
                uart_write('E');
            }
        }

        // Else write to PFM
        else
        {
            // Assume address is the start of a row
            // Write all of our data one row at a time
            unsigned char * row_start = command_data;
            while (length > 0)
            {
                nvm_write_row(address, row_start);

                // Increment address 1 row(64 words), word_start by 64*2 bytes
                address += 64;
                row_start += 128;

                // Subtract written data bytes from length
                length -= 128;
            }
            uart_write('K');
        }
    break;

    case 'D':   // Erase row
        // Address argument (2 bytes)
        uart_read_bytes(2, command_buff);
        address = (unsigned int)((command_buff[0] << 8) | command_buff[1]);
        nvm_erase(address);
        uart_write('K');
    break;

    default:    // Unknown command
        uart_write('?'); // Return question mark
    break;
    }

    return ret;
}

// EOF //
