#include "msp430f5529.h"

/**
 * Timer experiments.
 * Adapted by Ryan Fleck - Ryan.Fleck@protonmail.com
 *
 * Program functions:
 *  0. Set up timer module.
 *  1. Increases core voltage to Level 3.
 *  2. Increases clock speed to 25MHz.
 *  3. Blinks LED2 at P4.7 @6 Hz
 */
void SetVcoreUp (unsigned int level);

volatile unsigned int counter;

int main(void)
{
    // Stop watch-dog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Set the seventh bit of the P4 Data Direction Register to HIGH
    // This allows us to use P4.7 LED as an OUTPUT.
    P4DIR |= BIT7;
    P4OUT &= ~BIT7; // Turns LED off.
    P1DIR |= BIT0;
    P1OUT |= BIT0; // Turns LED off.
    counter = 0;

    /**
     * TIMER CONFIGURATION
     * ===================
     * https://ryanfleck.ca/2020/msp430-timer-module/
     */

    // Configure Timer A0.
    TA0CTL = TASSEL_2 + MC_2 + TACLR + ID_3;
    // From TI example ta0_02:
    // - SMCLK set as clock.
    // - MC - mode control - timer counts up to FFFF and resets.
    // - Clears TAR, previous count, clock divider. (TACLR)
    // - Sets prescaler to 8. (ID_3)

    TA0EX0 = TAIDEX_7;
    // - Sets external prescaler to 8, total of 64

    // For counting, we want output-compare mode.
    // Configure Timer A0 Channel 1:
    TA0CCTL1 = CCIE;
    TA0CCR1 = 0xFFFF;

    /**
     * With these settings, the LED will blink at 6 Hz.
     * Clock speed = 25000000 Hz
     * Prescaler = Internal 8 * External 8 = 64 Total
     * Timer counts to 65535 (0xFFFF) before interrupting
     * 
     * 25000000 / ( 64 * 65535 ) = 5.960555428397039749
     * -> Light will blink every 0.1677696‬ seconds, ~6Hz.
     */


    /**
     * CLOCK SPEED CONFIGURATION
     * =========================
     */

    /* Increase Vcore one step at a time. */
    SetVcoreUp (0x01);
    SetVcoreUp (0x02);  
    SetVcoreUp (0x03);  

    UCSCTL3 = SELREF_2; // Set DCO FLL reference = REFO
    UCSCTL4 |= SELA_2; // Set ACLK = REFO
  
    __bis_SR_register(SCG0); // Disable the FLL control loop
    UCSCTL0 = 0x0000; // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_7; // Select DCO range 50MHz operation
    UCSCTL2 = FLLD_0 + 762; // Set DCO Multiplier for 25MHz
    
    /**
     * (N + 1) * FLLRef = Fdco
     * (762 + 1) * 32768 = 25MHz
     * Set FLL Div = fDCOCLK/2
     * 
     * Worst-case settling time for the DCO when the DCO range bits have been
     * changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
     * UG for optimization.
     * 32 x 32 x 25 MHz / 32,768 Hz ~ 780k MCLK cycles for DCO to settle
     */
    __bic_SR_register(SCG0); // Enable the FLL control loop
    __delay_cycles(782000);
  
    // Loop until XT1,XT2 & DCO stabilizes
    // -> In this case only DCO has to stabilize
    do
    {
      // Clear XT2,XT1,DCO fault flags
      UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
      
      // Clear fault flags
      SFRIFG1 &= ~OFIFG;
      
    }while (SFRIFG1&OFIFG); // Test oscillator fault flag
    
    // TIMER CONFIGURATION: Begin waiting for timer interrupts.
    _BIS_SR(LPM3_bits + GIE);
}



/* TimerA has two interrupt vectors:
 *  - One dedicated to CCR0
 *  - One shared by TAIFG, plus all other channels.
 *  - A case statement is used to check which channel
 *    the interrupt was issued from for ch. 1-4.
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    // Handle CCR0 interrupts.
    return;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    switch (TA0IV)
    {
    case TA0IV_NONE:
        break;
    case TA0IV_TACCR1:
        counter++;
        if( counter > 5 ){
          P4OUT ^= BIT7; // Toggle the LED bit.
          P1OUT ^= BIT0;
          counter = 0;
        }
        break;
    case TA0IV_TACCR2:
        break;
    case TA0IV_TACCR3:
        break;
    case TA0IV_TACCR4:
        break;
    default:
        break;
    }
}



/**
 * From TI example MSP430F55xx_UCS_10.c
 * Increses CPU voltage.
 */
void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write
  PMMCTL0_H = PMMPW_H;
      
  // Set SVS/SVM high side new level
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
  
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
    
  // Set SVS/SVM low side to new level
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
  
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
}
