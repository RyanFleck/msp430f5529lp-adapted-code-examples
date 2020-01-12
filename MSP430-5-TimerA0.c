#include "msp430f5529.h"

/**
 * Timer experiments.
 * Adapted by Ryan Fleck - Ryan.Fleck@protonmail.com
 *
 * Switch at P1.1 blinks LED2 at P4.7
 */

void main(void)
{

    // Stop watch-dog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Set the seventh bit of the P4 Data Direction Register to HIGH
    // This allows us to use P4.7 LED as an OUTPUT.
    P4DIR |= BIT7;
    P4OUT &= ~BIT7; // Turns LED off.

    // Configure Timer A0.
    TA0CTL = TASSEL_2 + MC_2 + TACLR;
    // From TI example ta0_02:
    // - SMCLK set as clock.
    // - MC - mode control - timer counts up to FFFF and resets.
    // - Clears TAR, previous count, clock divider.
    TA0CTL |= BIT7 + BIT6;
    // - Sets prescaler to 8.

    // Configure Timer A0 Channel 0:
    //TA0CCR0 = 0xFFFF;

    // For counting, we want output-compare mode.

    // Configure Timer A0 Channel 1:
    TA0CCTL1 = CCIE;
    TA0CCR1 = 0xFFFF;

    TA0CCTL2 = CCIE;
    TA0CCR2 = 0xFFFA;

    // Enable all interrupts.
    __bis_SR_register(GIE);

    while (1)
        ;
}

/* TimerA has two interrupt vectors:
 *  - One dedicated to CCR0
 *  - One shared by TAIFG, plus all other channels.
 */

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{

}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    P1OUT ^= 0x01;                            // Toggle P1.0
    // Allows the compiler to generate more efficient code for the switch.
    // source: https://e2e.ti.com/support/microcontrollers/msp430/f/166/t/557522?What-does-the-even-in-range-function-do-
    switch (__even_in_range(TA0IV, TA0IV_TAIFG))
    {
    case TA0IV_NONE:
        break;
    case TA0IV_TACCR1:
        P4OUT ^= BIT7; // Toggle the LED bit.
        break;
    case TA0IV_TACCR2:
        break;
    case TA0IV_TACCR3:
        break;
    case TA0IV_TACCR4:
        break;
    default:
        __never_executed();
    }
}
