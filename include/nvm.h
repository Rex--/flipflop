
#ifndef _NVM_H_
#define _NVM_H_


unsigned int
nvm_flash_read (unsigned int address);

unsigned int
nvm_config_read (unsigned int address);
void
nvm_config_write (unsigned int address, unsigned int word);

void
nvm_flash_row (unsigned int address, unsigned char *row_data);

void
nvm_flash_erase (unsigned int address);

/**
 * Read a byte of data from EEPROM.
 * 
 * There are 256 bytes of EEPROM available for use. The addresses map to
 * 0xF000-0xF0FF in the flash.
 * 
 * @param[in]   address EEPROM memory address to read.
 * 
 * @returns     Byte of data read.
*/
unsigned char
nvm_eeprom_read (unsigned char address);


/**
 * Write a byte of data to EEPROM.
 * 
 * @param[in]   address EEPROM memory address to write.
 * @param[in]   data    Byte of data to write.
*/
void
nvm_eeprom_write (unsigned char address, unsigned char data);

#endif

// EOF //