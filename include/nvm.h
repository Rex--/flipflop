
#ifndef _NVM_H_
#define _NVM_H_


int
nvm_read_words (unsigned int address, int len, unsigned char * buff);

void
nvm_write (unsigned int address, unsigned int word);

void
nvm_write_row (unsigned int address, unsigned char *row_data);

void
nvm_erase (unsigned int address);

#endif

// EOF //