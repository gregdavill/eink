#include <msp430.h>
#include "main.h"
#include "eink.h"
#include "delay.h"




//Constants! ^_^
const char *days[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

//bad coding alert! - To save space all text printed to the bottom line is stored in reverse.
const unsigned char months[12][4] =
{"naJ","beF","raM","rpA","yaM","nuJ","luJ","guA","peS","tcO","voN","ceD"};

const unsigned char Month_Days [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const unsigned char Cent_Table [] = {6,4,2,0};
const unsigned char Mont_Table [] = {0,3,3,6,1,4,6,2,5,0,3,5,6,2};



#define TESTMODE

// Flags
volatile unsigned char updateFlag;


volatile unsigned char state;

//Config Variables



// global time variables
unsigned int gMonth = 10,gDay = 2,gSecond,gMinute = 0,gHour = 18;
unsigned int gYear = 2012;



unsigned char getDow(void);

void wake(void);
void sleep(void);


const int set_bounds[] = {59, 23, 0, 12, 9999};
int *set_table[] = {(int*)&gMinute,(int*)&gHour,(int*)&gDay,(int*)&gMonth,(int*)&gYear};
char set;


int main(void)
{
//	BCSCTL3 |= LFXT1S_2; 
	WDTCTL = WDTHOLD+WDTPW;
	
	DCOCTL = 0x00;							// Set DCOCLK to 1MHz
	
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;
	
	displayInverse = 1;
	updateFlag = 1;
	
	wake();
	
	P1REN = P1IE = P1IES = BUTpin;
	P1IFG = 0;
	
	__bis_SR_register(GIE);
		

	updateFlag = 1;
	
	while(1)
	{	
		
		if(updateFlag)
		{
			einkClearData();
		
			unsigned char dow = getDow();
			if(dow == 0 || dow == 6)
				displayInverse = 1;
			else
				displayInverse = 0;
			
			einkPuts((char*)&days[dow][0],10);
			einkPuts((char*)&months[gMonth][0],4);
	
			display_data[0] = gYear%10 	  	 + '0'-' ';
			display_data[1] = gYear/10%10 	 + '0'-' ';
			display_data[2] = gYear/100%10 	  	 + '0'-' ';
			display_data[3] = gYear/1000%10 	 + '0'-' ';
			//display_data[2] = '-'-' ';

			display_data[8] = (gDay+1)%10 	 + '0'-' ';
			display_data[9] = (gDay+1)/10%10 + '0'-' ';

			einkUpdate();
			
			updateFlag = 0;
		}
		
		if(!updateFlag)
		{
			sleep();
			wake();
		}
	}
}



// Mildly Complicated Day of Week lookup system
// based on the methods outlined in wikipedia article
// Parameter ranges
// gMonths 0-11
// gDays 0->∞
// gYears 0->∞
unsigned char getDow(void){
	return( ( ( ( Cent_Table[ ( gYear/100 ) % 4 ] ) ) + ( gYear%100 ) + ( (gYear%100)/4 ) + ( ( Mont_Table[ ( ( ( gYear % 4 == 0 ) && ( gMonth == 0 || gMonth == 1 ) ) ? gMonth+12 : gMonth ) ] ) ) + gDay + 1 ) % 7 );
}

void wake(void)
{
	P1DIR = EIO1pin + XCKpin + LATCHpin + SLEEPBpin + DI0pin + VCCpin + GNDpin;							// P1.6 output (green LED)
	P1OUT = EIO1pin + SLEEPBpin + DI0pin + VCCpin + BUTpin;								// red LED on

	
	BCSCTL1 = DIVA_3;		// ACLK/8
	BCSCTL2 = XCAP_2;		// MCLK = SCLK = SMCLK ~8Mhz

	TACTL = TASSEL_1 + ID_3 + MC_2;     	      	// ACLK/8, cont-mode
	CCTL0 = CCIE;

}

void sleep(void)
{
	
	P1OUT = BUTpin;
	P1DIR = GNDpin;
	
	BCSCTL2 = SELM_3 + SELS + XCAP_2; // MCLK = SCLK = ACLK/8

	__bis_SR_register(LPM3_bits);
	__no_operation();
}


// TimerA2 CCR1 or Overflow Interrupt
__attribute__((interrupt(TIMERA0_VECTOR))) 
void TimerA0(void)
{
	CCTL0 &= ~(CCIFG); 	// Clear Compare interrupt and interrupt flag.
	
		gSecond++;
		if(gSecond >= 10)
		{
			gSecond = 0;
#ifndef TESTMODE
			gMinute++;
			if(gMinute >= 15)
			{
				gMinute = 0;
				gHour++;
				if(gHour >= 23)
				{
					gHour = 0;
#endif
					updateFlag++;
					gDay++;
					if(gDay >= Month_Days[gMonth])
					{
						gDay = 0;
						gMonth++;
						if(gMonth >= 11)
						{
							gMonth = 0;
							gYear++;
						}
					}				

				}

#ifndef TESTMODE 

			}
		}
#endif

		CCR0 += 32768-1;
		if(updateFlag)
		{
			__bic_SR_register_on_exit(LPM3_bits);
		//	P1OUT ^= 0x01;
		}
	
	
}


__attribute__((interrupt(PORT1_VECTOR))) 
void P1int(void)
{
	P1IFG = 0;
	P1IE = BUTpin;
	



	gDay++;

	if(set)
	{

		
			*set_table[set-1] += (set == 1) ? 15 : 1;
			if(set != 3) // days have different bounds
			{
				if(*set_table[set-1] >= set_bounds[set-1])
					*set_table[set-1] = 0;
			}
			else
			{
				if(gDay >= Month_Days[gMonth])
				{
					gDay = 0;
				}	
			}
			
			CCR0 += 49152; // reset timeout counter

			updateFlag = 1;
			
			
	}else
	updateFlag++;


	__bic_SR_register_on_exit(LPM4_bits);
}

