#include <msp430.h>
#include "spi.h"
#include "eink.h"
#include "main.h"


unsigned char modeMSB;

void spiTx(int d,int c)
{
	char cnt;
	for(cnt = 0;cnt < c;cnt++)
	{

			if(d & (modeMSB ? 0x8000 : 0x0001))
				P1OUT |= DI0pin;
			else
				P1OUT &= ~DI0pin;
			
			P1OUT |= XCKpin;
			
			if(modeMSB)
				d <<= 1;
			else
				d >>= 1;
				
			P1OUT ^= XCKpin;
			
	}
}