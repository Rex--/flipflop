
#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

// If the bootloader strapping pin is left undefined, it defaults to always
// entering the bootloader after a reset.
#ifndef BOOTLOADER_PIN
#   define BOOTLOADER_PIN 1
#endif

void
bootloader_start (void);

#endif

// EOF //
