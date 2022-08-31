/**
 * Main boot loader application. 
*/

#include <xc.h>

#include "bootloader.h"
#include "uart.h"
#include "nvm.h"

// Buffer that holds a commmand (1 byte) + its arguments (max 2 bytes).
static unsigned char command_buff[3];

// Buffer that holds command data. (max 64words*2bytes/word)
unsigned char command_data[128];

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

    switch (command_buff[0])
    {
    unsigned int address;
    unsigned int data;

    case 'U':   // Reset device
        RESET();
    break;

    case 'X':   // Start user app
        ret = 1;
    break;

    case 'R':   // Read word
        // 14-bit Address argument (2 bytes)
        for (char arg = 1; arg < 3; arg++)
        {
            command_buff[arg] = uart_read();
        }
        data = nvm_flash_read((unsigned int)((command_buff[1] << 8) | command_buff[2]));
        uart_write((unsigned char)(data >> 8));
        uart_write((unsigned char)(data & 0xFF));
    break;

    case 'C':   // Write word
        // 14-bit Address argument (2 bytes)
        for (char arg = 1; arg < 3; arg++)
        {
            command_buff[arg] = uart_read();
        }
        address = (unsigned int)((command_buff[1] << 8) | command_buff[2]);

        // Word to write (2 bytes)
        for (char dat = 0; dat < 2; dat++)
        {
            command_data[dat] = uart_read();
        }
        data = (unsigned int)((command_data[0] << 8) | command_data[1]);

        nvm_config_write(address, data);

        uart_write('K');
    break;

    case 'P':   // Program row
        // 14-bit Address argument (2 bytes)
        for (char arg = 1; arg < 3; arg++)
        {
            command_buff[arg] = uart_read();
        }

        // 64 words of data (128 bytes)
        for (char dat = 0; dat < 128; dat++)
        {
            command_data[dat] = uart_read();
        }

        // Write row of data
        address = (unsigned int)((command_buff[1] << 8) | command_buff[2]);
        nvm_flash_row(address, command_data);

        // Verify Row

        // Return response
        uart_write('K');
    break;

    case 'D':   // Erase row
        // Address argument (2 bytes)
        for (char arg = 1; arg < 3; arg++)
        {
            command_buff[arg] = uart_read();
        }
        address = (unsigned int)((command_buff[1] << 8) | command_buff[2]);
        nvm_flash_erase(address);
        uart_write('K');
    break;

    default:    // Unknown command
        uart_write('?'); // Return question mark
    break;
    }

    return ret;
}

// EOF //
