#include <msp430.h>
#include "delay.h"

volatile unsigned char delayOver;

void delay(int y)
{
	CCR1 = TAR + y;
	CCTL1 = CCIE;
	delayOver = 0;
	while(delayOver == 0)
		__bis_SR_register(LPM0_bits+GIE);
}


// TimerA2 CCR1 or Overflow Interrupt
__attribute__((interrupt(TIMERA1_VECTOR))) 
void TimerA1(void)
{
	CCTL1 &= ~(CCIFG + CCIE); 	// Clear Compare interrupt and interrupt flag.
	
	delayOver = 1;			// Set delay finish Flag
	
	__bic_SR_register_on_exit(LPM3_bits);
}
