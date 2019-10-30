#include "msp430f5529.h"
#include <stdbool.h>

/**
 * button.c - Polling edition.
 * Adapted by Ryan Fleck - Ryan.Fleck@protonmail.com
 *
 * Button at P1.1 enables LED2 at P4.7
 */

void main(void)
{
    volatile unsigned int i;

    // Stop watch-dog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Set the seventh bit of the P4 Data Direction Register to HIGH
    // This allows us to use P4.7 LED as an OUTPUT.
    P4DIR |= BIT7;

    // Button setup:
    P1DIR &= ~BIT1; // P1.1 set to input. Other P1 bits unaffected.

    // I understand that we need to enable and do something with the
    // pulldown resistor, but I need to read more to see exactly what's
    // going on with these two steps.
    P1OUT |= BIT1;
    P1REN |= BIT1;

    while (1)
    {

        if ((P1IN & BIT1) == BIT1)
        {
            P4OUT &= ~BIT7;
        }
        else
        {

            P4OUT |= BIT7;
        }

    }
}

