#include "msp430f5529.h"

void setup() {
  // put your setup code here, to run once:
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly: 
  P4OUT |= BIT7;
  delay(1000);
  
  P4OUT &= ~BIT7;
  delay(1000);
}
