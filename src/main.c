/**
 * Main loop.
 * This file holds the code that runs on reset and decides whether to start the
 * bootloader or launch the user application. 
*/

#include <xc.h>

#include "boot_config.h"
#include "bootloader.h"

#include "nvm.h"

// Configuration words for software control of watchdog timer.
#pragma config WDTCCS   = SC
#pragma config WDTCWS   = WDTCWS_7
#pragma config WDTE     = SWDTEN
#pragma config WDTCPS   = WDTCPS_31

// These variables hold the POR values of registers we use.
static volatile unsigned char por_osc = 0;
static volatile unsigned char por_hfosc = 0;

void
main (void)
{
    // Watchdog was not cause of reset, meaning we start bootloader
    if (1 == PCON0bits.nRWDT)
    {

        // Set watchdog prescaler to ~16s
        WDTCON0bits.WDTPS = 0b01110;

        // Enable WDT
        WDTCON0bits.SWDTEN = 1;

        // unsigned char new_osc = 0;

        // Save value of clock:divider at POR
        por_osc = OSCCON2;

        // Save value of HFINTOSC
        por_hfosc = OSCFRQ;

        // if (OSCCON2bits.COSC != 0b110)
        // {
        //     // Set clock source to HFINTOSC if not already.
        //     new_osc |= (0b110 << 4);
        // }

        // if (OSCCON2bits.CDIV != 0)
        // {
            // Set clock divider to 1:1 if not
            // (Default value of new_osc)
            // new_osc |= 0;
        // }

        // Set NOSC register to HFINTOSC with 1:1 divider.
        OSCCON1 = 0b1100000;

        // Set HFINTOSC to our needed speed (20 mhz?)
        OSCFRQ = 0b110;

        bootloader_start();

        //  Reset used registers back to POR value
        OSCCON1 = por_osc;
        OSCFRQ = por_hfosc;

        // Disable watchdog
        WDTCON0bits.SWDTEN = 0;

        // Reset prescaler
        WDTCON0bits.WDTPS = 0b01011;
    }

    // Jump to application reset vector
    asm("ljmp " ___mkstr(PROG_RST_VECT));
}

// EOF //
