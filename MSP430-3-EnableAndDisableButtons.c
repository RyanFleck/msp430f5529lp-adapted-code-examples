#include "msp430f5529.h"
#include <stdbool.h>

/**
 * EnableAndDisableButtons.c - Polling edition.
 * Adapted by Ryan Fleck - Ryan.Fleck@protonmail.com
 *
 * Blinks LED2 at P4.7
 */

void main(void)
{
    volatile unsigned int i;

    // Stop watch-dog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Set the seventh bit of the P4 Data Direction Register to HIGH
    // This allows us to use P4.7 LED as an OUTPUT.
    P4DIR |= BIT7;
    P1DIR |= BIT0;

    // Button setup:
    P1DIR &= ~BIT1;
    P1OUT |= BIT1;
    P1REN |= BIT1;

    // Second button setup
    P2DIR &= ~BIT1; // Bit 1 of Register 2 is zero (input)
    P2OUT |= BIT1;  // Pull-up mode is PULL-UP to 5v.
    P2REN |= BIT1;  // Enable internal pull-up.

    while (1)
    {

        // When the buton is depressed, it will register as a ZERO.
        // When bitwise AND between P1IN and BIT1 is NOT one, we want to activate this.
        if (!(P1IN & BIT1))
        {
            // Enable the LED2 by toggling bit 7 ON.
            P4OUT |= BIT7;
            // Disable LED1
            P1OUT &= ~BIT0;
        }
        else if(!(P2IN & BIT1))
        {
            // Disable the LED2 by toggling bit 7 OFF,
            // What this really does is AND with the 1C of BIT7, allowing
            // everything but BIT7 to stay on in the P4OUT register.
            P4OUT &= ~BIT7;
            // Enable the LED1
            P1OUT |= BIT0;
        }

    }
}

