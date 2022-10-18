/** 
 * This file contains code related to calculating and certifying CRCs.
 * 
 * We currently use a very simple and slow method. I don't think we have
 * enough available flash to store a lookup table. We could generate one on
 * startup and store it in RAM, but we might not have enough to spare on
 * smaller devices (like the one being developed on).
 * 
 * CRC Algorithm ::
 *  Name    : CRC-16/CCITT
 *  Width   : 16
 *  Poly    : x1021
 *  Init    : xFFFF
 *  RefIn   : False
 *  RefOut  : False
 *  XorOut  : x0000
 *  Check   : x29B1
*/

#include <xc.h>
#include "crc.h"

#define POLYNOMIAL 0x1021
#define WIDTH  (unsigned int)(8 * sizeof(unsigned int))
#define TOPBIT (unsigned int)(1 << (WIDTH - 1))

unsigned int
crc_crc16 (unsigned char * message, int nBytes)
{
    unsigned int remainder = 0xffff;

    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (int byte = 0; byte < nBytes; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (unsigned int)(message[byte] << (WIDTH - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (unsigned char bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return (remainder);

}

// EOF //
