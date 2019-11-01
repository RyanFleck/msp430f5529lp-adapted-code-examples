#include "msp430f5529.h"

/**
 * blink.c
 * Adapted by Ryan Fleck - Ryan.Fleck@protonmail.com
 *
 * Blinks LED2 at P4.7
 */

void main(void)
{
    // Stop watch-dog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Set the seventh bit of the P4 Data Direction Register to HIGH
    // This allows us to use P4.7 as an OUTPUT.
    P4DIR |= 0x80;

    volatile unsigned int i;

    while (1)
    {
        // XOR the P4 register, switching the direction of the 7th (last) bit
        P4OUT ^= 0x80;
        
        // Burn some cycles to delay.
        for (i = 90000; i > 0; i--)
            ;
    }
}

