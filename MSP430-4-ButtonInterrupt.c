#include "msp430f5529.h"

/**
 * Blink.c - Interrupt edition.
 * Adapted by Ryan Fleck - Ryan.Fleck@protonmail.com
 *
 * Switch at P1.1 blinks LED2 at P4.7
 */

void main(void)
{
    volatile unsigned int i;

    // Stop watch-dog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Set the seventh bit of the P4 Data Direction Register to HIGH
    // This allows us to use P4.7 LED as an OUTPUT.
    P4DIR |= BIT7;
    P4OUT &= ~BIT7; // Turns LED off.

    // Button setup:
    P1DIR &= ~BIT1; // P1.1 is the input button, set bit to 0.
    P1REN |= BIT1;  // Set pullup resistor to enabled.
    P1OUT |= BIT1;  // Set pullup resistor to active mode.
    P1IE |= BIT1;   // P1.1 interrupt enable
    P1IFG &= ~BIT1; // P1.1 clear interrupt flag.

    // Enable all interrupts.
    __bis_SR_register(GIE);

    while (1);
}

/*
 * The '#pragma vector=' places a reference to this function in the
 * interrupt vector table. When PORT1 is triggered by an interrupt, this
 * function runs.
 */
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    P4OUT ^= BIT7; // Toggle the LED bit.
    P1IFG &= ~BIT1; // Remove the interrupt flag so another can be triggered.
}


