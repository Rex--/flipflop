#include <xc.h>

#include "nvm.h"
#include "uart.h"

// NVM unlock sequence. This will set the WR bit and write any data.
static void nvm_unlock (void);

unsigned int
nvm_flash_read (unsigned int address)
{
    if (address > 0x7FFF)
    {
        // Set bit to indicate were reading config/ID/DIA/EEPROM
        NVMCON1bits.NVMREGS = 1;
    }
    else
    {
        // Clear bit to indicate were reading flash
        NVMCON1bits.NVMREGS = 0;
    }

    // Load address into NVMADR registers
    NVMADRH = address >> 8;
    NVMADRL = address & 0xFF;

    // Set bit to initiate read
    NVMCON1bits.RD = 1;

    // Data is available the very next cycle.
    return (unsigned int)((NVMDATH << 8) | NVMDATL);
}

void
nvm_flash_row (unsigned int address, unsigned char *row_data)
{
    // Before writing we have to erase the row
    nvm_flash_erase(address);

    // Clear bit to indicate we're writing to PFM
    NVMCON1bits.NVMREGS = 0;

    // Enable NVM writes
    NVMCON1bits.WREN = 1;

    // Set flag to only load data latches
    NVMCON1bits.LWLO = 1;

    // Load starting address into NVMADR registers
    NVMADRH = (address >> 8);
    NVMADRL = (address & 0xFF);

    // Load 63 words into the data latches
    for (unsigned char word = 0; word < 63; word++)
    {
        // Load word into NVM registers
        NVMDATH = *row_data++;
        NVMDATL = *row_data++;

        // Initiate write sequence
        nvm_unlock();

        // Increment word address
        address++;
        NVMADRH = (address >> 8);
        NVMADRL = (address & 0xFF);
    }

    // Clear flag to enable flash write
    NVMCON1bits.LWLO = 0;

    // Load final word
    NVMDATH = *row_data++;
    NVMDATL = *row_data;

    // Initiate write sequence. This will write the entire row to flash.
    nvm_unlock();
}


void
nvm_config_write (unsigned int address, unsigned int word)
{
    // Set bit to indicate we're writing to config
    NVMCON1bits.NVMREGS = 1;

    // Enable NVM writes
    NVMCON1bits.WREN = 1;

    // Load address into NVMADR registers
    if ((address >> 8) == 0xF0)
    {
        // Writing EEPROM
        NVMADRH = 0x70;
    }
    else
    {
        // Writing CONFIG/ID
        NVMADRH = 0;
    }
    NVMADRL = (address & 0xFF);

    // Load word into NVM registers
    NVMDATH = word >> 8;
    NVMDATL = word & 0xFF;

    // Initiate write sequence
    nvm_unlock();
}

void
nvm_flash_erase (unsigned int address)
{
    if (address > 0x7FFF)
    {
        // Set bit to indicate we're reading config/ID/DIA
        NVMCON1bits.NVMREGS = 1;
        // address = address & 0xFF;
    }
    else
    {
        // Clear bit to indicate we're reading flash
        NVMCON1bits.NVMREGS = 0;
    }

    // Enable writes
    NVMCON1bits.WREN = 1;

    // Set FREE bit to indicate erasure
    NVMCON1bits.FREE = 1;

    // Load address
    NVMADRH = (address >> 8);
    NVMADRL = (address & 0xFF);

    // Initiate erase
    nvm_unlock();

    // Disable write
    NVMCON1bits.WREN = 0;
}

static void
nvm_unlock (void)
{
    // The unlock sequence must not be interrupted, so we disable interrupts.
    // unsigned char gie_bit = PIE0bits.GIE;
    // PIE0bits.GIE = 0;

    // Step 1. Write 0x55 to NVMCON2
    NVMCON2 = 0x55;

    // Step 2. Write 0xAA to NVMCON2
    NVMCON2 = 0xAA;

    // Step 3. Set WR bit of NVMCON1
    //  This will start the write.
    NVMCON1bits.WR = 1;

    // Unlock sequence complete, we may reenable interrupts.
    // PIE0bits.GIE = gie_bit;
}


// EOF //
