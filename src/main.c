#include <msp430f2013.h>

#define EIO1pin 	0x40 	// Input/output pin for chip selection
#define XCKpin 		0x20	// Clock input pin for taking display data
#define LATCHpin 	0x08	// Latch pulse input pin for display data
#define SLEEPBpin 	0x04  	// 
#define DI0pin  	0x02	// Input pin for display data
#define VCCpin 		0x10	// Power on display connected here
#define GNDpin 		0x01	// Power on display connected here

#define BUTpin		0x80


//	Display Buffer
unsigned char display_data[20];

/*

 "Compressed" font data, in binary form 0b ABCD EFGH IJKL MNOP
  {A-P} represent segment bits in each character
		 
	     __J_ _A__ 
		|\   |   /| 
	   	| K  H  B |
		L  \ | /  C
		|   \|/   |
		 -M-- --D- 
		|   /|\   |
		N  / | \  E
		| O  I  F |
		|/   |   \|
		 ~~P~ ~G~~		
		
	Stored in infomem[D,C,B]
*/		

const unsigned int font_data[]
__attribute__((section(".infomem"))) = { 
0x0000,0xC101,0x0110,0x3B89,0x9BD9,0x5BDA,0x0F6D,0x4000,
0x4400,0x0022,0x55AA,0x1188,0x0002,0x1008,0x0001,0x4002,
0xEA57,0x6800,0xB24D,0xDA41,0x3818,0x8659,0x9A5D,0xA840,
0xBA5D,0xBA59,0x0041,0x0042,0x4203,0x1209,0x0621,0xB0C0,
0xAACD,0xB85C,0xBBC1,0x8255,0xABC1,0x825D,0x805C,0x9A55,
0x381C,0x83C1,0x81C5,0x441C,0x0215,0x6834,0x2C34,0xAA55,
0xB05C,0xAE55,0xB45C,0x9A59,0x81C0,0x2A15,0x4016,0x2C16,
0x4422,0x40A0,0xC243,0x0055,0x0420,0xAA00,0x0402,0x0201,
0x0020,0x028D,0x009D,0x000D,0x018D,0x000F,0x9188,0x1E01,
0x009C,0x0080,0x0081,0x1580,0x03C0,0x188C,0x008C,0x008D,
0x015C,0x03D8,0x000C,0x1081,0x1388,0x0085,0x0006,0x0A85,
0x4182,0x0E01,0x000B,0x8388,0x0180,0x11C1,0x3150,0x0000,
};

//Constants! ^_^
const char *days[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

//bad coding alert! - To save space all text printed to the bottom line is stored in reverse.
const unsigned char months[12][4] =
{"naJ","beF","raM","rpA","yaM","nuJ","luJ","guA","peS","tcO","voN","ceD"};

const unsigned char Month_Days [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

const unsigned char Cent_Table [] = {6,4,2,0};
const unsigned char Mont_Table [] = {0,3,3,6,1,4,6,2,5,0,3,5,6,2};


// Flags
volatile unsigned char delayOver,setTimeout,updateFlag;


volatile unsigned char state;

//Config Variables
unsigned char displayInverse , modeMSB;


// global time variables
unsigned int gMonth = 9,gDay = 11,gSecond,gMinute = 0,gHour = 18;
unsigned int gYear = 2011;


void puts(const char* str,char l);
unsigned char getDow(void);
void invert(void);
void wake(void);
void sleep(void);
void eUpdate(void);
void Xmit(int f,int b);
void usiXmit(int d,int c);
void delayTicks(int y);
void clearData(void);

const int set_bounds[] = {59, 23, 0, 12, 9999};
int *set_table[] = {(int*)&gMinute,(int*)&gHour,(int*)&gDay,(int*)&gMonth,(int*)&gYear};
char set;


int main(void)
{
//	BCSCTL3 |= LFXT1S_2; 
	WDTCTL = WDTHOLD+WDTPW;
	
	DCOCTL = 0x00;							// Set DCOCLK to 1MHz
	
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	
	displayInverse = 0;
	updateFlag = 1;
	
	wake();
	
	P1REN = P1IE = P1IES = BUTpin;
	P1IFG = 0;
	
	__bis_SR_register(GIE);
		
	set = 5;
	
	while(set > 0)
	{
		
		Xmit(0xFFFF,4);
		delayTicks(12000);
		
		CCR0 = TAR - 1;
		CCTL0 = CCIE;
		setTimeout = 1;
		
		updateFlag = 1; 
		while(1)
		{	
			while(updateFlag)
			{
			
			updateFlag = 0;
			
			display_data[10] = (gDay+1)/10%10 + '0'-' ';
			display_data[11] = (gDay+1)%10 + '0'-' ';
				
			display_data[13] = (gMonth+1)/10%10 + '0'-' ';
			display_data[14] = (gMonth+1)%10 + '0'-' ';
				
			display_data[16] = gYear/1000%10 + '0'-' ';
			display_data[17] = gYear/100%10 + '0'-' ';
			display_data[18] = gYear/10%10 + '0'-' ';
			display_data[19] = gYear%10 + '0'-' ';
				
			display_data[12] = display_data[15] = '/'-' ';
			
			
			display_data[7] = gHour/10%10 + '0'-' ';
			display_data[6] = gHour%10 + '0'-' ';
			
			display_data[5] = ':'-' ';
			
			display_data[4] = gMinute/10%10 + '0'-' ';
			display_data[3] = gMinute%10 + '0'-' ';
			
			Xmit(0xFFFF,2);
			delayTicks(3000);
			Xmit(0,4);
			delayTicks(3000);
			Xmit(0xFFFF,1);
			delayTicks(3000);
			Xmit(0,3);
			delayTicks(3000);
			
			//eUpdate();
			
			
			}
				
			LPM0;	// only two wakeup sources.
					// as the timere will set delayOver no test is needed.
		
			if(setTimeout == 0)
				break;
		
				
			
		}
		
		
		
		set--; 		// move to the next item on the list
	}
	
	
//	Temp = 15;
	
	clearData();
	eUpdate();
	
	
//	sleep();

//	Temp = 20;
	
		
	updateFlag = 1;
	
	while(1)
	{	
		
		if(updateFlag)
		{
			clearData();
		
			unsigned char dow = getDow();
			
			puts((char*)&days[dow][0],10);
			puts((char*)&months[gMonth][0],4);
	
			display_data[0] = gYear%10 	  	 + '0'-' ';
			display_data[1] = gYear/10%10 	 + '0'-' ';
			display_data[2] = '-'-' ';

			display_data[8] = (gDay+1)%10 	 + '0'-' ';
			display_data[9] = (gDay+1)/10%10 + '0'-' ';

			eUpdate();
			
			updateFlag--;
		}
		
		if(updateFlag == 0)
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



void puts(const char* str,char l)
{
	int i;
	for(i = 0;i< 20; i++)
	{
		if(str[i] == ' ')
			display_data[i+l] = 0;
		else if(str[i] == 0)
			return;
		else
			display_data[i+l] = str[i] - ' ';
	}
}

void clearData(void)
{
	int i;
	for(i = 0;i< 20; i++)
	{
		display_data[i] = 0;
	}
}

void wake(void)
{
	P1DIR = EIO1pin + XCKpin + LATCHpin + SLEEPBpin + DI0pin + VCCpin + GNDpin;							// P1.6 output (green LED)
	P1OUT = EIO1pin + SLEEPBpin + DI0pin + VCCpin + BUTpin;								// red LED on

	
	TACTL = TASSEL_1 + ID_0 + MC_2;     	      	// SMCLK/8, cont-mode
	__bic_SR_register(SCG0);
	BCSCTL2 = 0; // MCLK, SMCLK = DCO (1MHz)
}

void sleep(void)
{
	P1OUT = BUTpin;
	P1DIR = 1;
	
	TACTL = TASSEL_1 + ID_3 + MC_2;     	      	// SMCLK/8, cont-mode
	CCTL0 = CCIE;
	CCR0 = TAR + 4096-1;
	
	BCSCTL2 = SELM_3 + SELS;
	__bis_SR_register(SCG0);	// disable DCO for lower power RTC


	__bis_SR_register(LPM3_bits);
	__no_operation();
}

void eUpdate(void){
		
	Xmit(0xFFFF,2);
	delayTicks(49152);
	Xmit(0,4);
	delayTicks(49152);
	Xmit(0xFFFF,1);
	delayTicks(49152);
	Xmit(0,3);
	delayTicks(49152);

}

void Xmit(int f,int b)
{
	
	int t,y;
	
	modeMSB = 1; 	// MSB first (flips bottom row characters)
	
	for(y = 0; y < 2; y++)
	{
		P1OUT &= ~EIO1pin;
		
		//usiXmit(displayInverse == 0 ? 0xFFFF : 0,1);
		usiXmit(0xFFFF,1);
		
		
		P1OUT |= EIO1pin;
		
		for(t = 0; t < 10; t++)
		{
			int d;
			int e = display_data[t+(10*y)];
		
			if(b == 1)
			{
				d = ~font_data[e];
			//	if(displayInverse)
			//		d = ~d;
			}
			else if(b == 2)
			{
				d = font_data[e];
			//	if(displayInverse)
			//		d = ~d;
			}
			else if(b == 3)
				d = 0;
			else
				d = 0xFFFF;
			
			usiXmit(d, 16);
		}
		usiXmit(f,1);
		
		modeMSB = 0; 	// LSB first (top data not flipped)
	}

	P1OUT |= LATCHpin;
	P1OUT &= ~LATCHpin;
}

void usiXmit(int d,int c)
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
				
			P1OUT &= ~XCKpin;
			
	}
}

void delayTicks(int y)
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


// TimerA2 CCR1 or Overflow Interrupt
__attribute__((interrupt(TIMERA0_VECTOR))) 
void TimerA0(void)
{
	CCTL0 &= ~(CCIFG); 	// Clear Compare interrupt and interrupt flag.
	
	
	if(setTimeout)
	{
		setTimeout = 0;
		CCTL0 &= ~(CCIE); 	// Clear Compare interrupt and interrupt flag.
		
		__bic_SR_register_on_exit(LPM0_bits);
	}

	if(set == 0)
	{
		gSecond++;
		if(gSecond >= 3)
		{
			gSecond = 0;
			gMinute++;
			if(gMinute >= 15)
			{
				gMinute = 0;
				gHour++;
				if(gHour >= 23)
				{
					gHour = 0;
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
			}
		}
		CCR0 -= 4096-1;
		if(updateFlag)
		{
			__bic_SR_register_on_exit(LPM3_bits);
		//	P1OUT ^= 0x01;
		}
	}
	
	
}


__attribute__((interrupt(PORT1_VECTOR))) 
void P1int(void)
{
	P1IFG = 0;
	P1IE = BUTpin;
	
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

	__bic_SR_register_on_exit(LPM3_bits);
}

