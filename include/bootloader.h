
#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

/**
 * Start the bootloader.
 * This starts the main commmand loop of the bootloader which receives a
 * command, then executes it. This function will only return when the command
 * to start the user application is received. Otherwise, the watchdog timeout
 * will cause a reset.
*/
void
bootloader_start (void);

#endif

// EOF //
