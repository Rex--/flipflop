/**
 * Main loop.
 * This file holds the code that runs on reset and decides whether to start the
 * bootloader or launch the user application. 
*/

#include <xc.h>

#include "boot_config.h"
#include "bootloader.h"

// Util functions for setting/unsetting state.
void clock_set (void);
void clock_reset (void);

// These variables hold the POR values of registers we use.
static volatile unsigned char por_osc = 0;
static volatile unsigned char por_hfosc = 0;

void
main (void)
{
    // We only start boot loader if RX is high.
    if (BOOTLOADER_PIN)
    {
        clock_set();
        bootloader_start();
        clock_reset();
    }

    // Jump to application reset vector
    asm("ljmp " ___mkstr(PROG_RST_VECT));

    while (1);
}

void
clock_set (void)
{
    unsigned char new_osc = 0;

    // Save value of clock:divider at POR
    por_osc = OSCCON2;

    // Save value of HFINTOSC
    por_hfosc = OSCFRQ;

    if (OSCCON2bits.COSC != 0b110)
    {
        // Set clock source to HFINTOSC if not already.
        new_osc |= (0b110 << 4);
    }

    if (OSCCON2bits.CDIV != 0)
    {
        // Set clock divider to 1:1 if not
        // (Default value of new_osc)
        // new_osc |= 0;
    }

    // Set NOSC register with our new clock:divider value.
    OSCCON1 = new_osc;

    // Set HFINTOSC to our needed speed
    OSCFRQ = 0b110;
}

void
clock_reset (void)
{
    //  Reset used registers back to POR value
    OSCCON1 = por_osc;
    OSCFRQ = por_hfosc;
}

// EOF //
