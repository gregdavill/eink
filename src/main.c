#include <msp430g2231.h>
void SPI(char);
void load(int,char);

unsigned char data[6*5 + 4*3];
const unsigned char font_data[]= {
0x08,0x08,0x08,0x08,0x08,/*-*/
0x00,0x00,0x06,0x06,0x00,/*.*/
0x02,0x0C,0x10,0x60,0x80,/* */
0x7C,0x8A,0x92,0xA2,0x7C,/*0*/
0x00,0x42,0xFE,0x02,0x00,/*1*/
0x42,0x86,0x8A,0x92,0x62,/*2*/
0x44,0x82,0x92,0x92,0x6C,/*3*/
0x10,0x30,0x50,0xFE,0x10,/*4*/
0xE4,0xA2,0xA2,0xA2,0x9C,/*5*/
0x3C,0x52,0x92,0x92,0x0C,/*6*/
0x80,0x86,0x98,0xE0,0x80,/*7*/
0x6C,0x92,0x92,0x92,0x6C,/*8*/
0x60,0x92,0x92,0x94,0x78,/*9*/
0x00,0x00,0x36,0x36,0x00,/*:*/
0x00,0x00,0x35,0x36,0x00,/*;*/
0x10,0x28,0x44,0x82,0x00,/*<*/
0x28,0x28,0x28,0x28,0x28,/*=*/
0x00,0x82,0x44,0x28,0x10,/*>*/
0x40,0x80,0x8A,0x90,0x60,/*?*/
0x7C,0x82,0xBA,0xBA,0x62,/*@*/
0x3E,0x48,0x88,0x48,0x3E,/*A*/
0xFE,0x92,0x92,0x92,0x6C,/*B*/
0x7C,0x82,0x82,0x82,0x44,/*C*/
0xFE,0x82,0x82,0x82,0x7C,/*D*/
0xFE,0x92,0x92,0x92,0x82,/*E*/
0xFE,0x90,0x90,0x90,0x80,/*F*/
0x7C,0x82,0x92,0x92,0x5E,/*G*/
0xFE,0x10,0x10,0x10,0xFE,/*H*/
0x82,0x82,0xFE,0x82,0x82,/*I*/
0x84,0x82,0xFC,0x80,0x80,/*J*/
0xFE,0x10,0x28,0x44,0x82,/*K*/
0xFE,0x02,0x02,0x02,0x02,/*L*/
0xFE,0x40,0x20,0x40,0xFE,/*M*/
0xFE,0x60,0x10,0x0C,0xFE,/*N*/
0x7C,0x82,0x82,0x82,0x7C,/*O*/
0xFE,0x90,0x90,0x90,0x60,/*P*/
0x7C,0x82,0x82,0x86,0x7E,/*Q*/
0xFE,0x90,0x98,0x94,0x62,/*R*/
0x64,0x92,0x92,0x92,0x4C,/*S*/
0x80,0x80,0xFE,0x80,0x80,/*T*/
0xFC,0x02,0x02,0x02,0xFC,/*U*/
0xF8,0x04,0x02,0x04,0xF8,/*V*/
0xFC,0x02,0x0C,0x02,0xFC,/*W*/
0xC6,0x28,0x10,0x28,0xC6,/*X*/
0xC0,0x20,0x1E,0x20,0xC0,/*Y*/
0x86,0x8A,0x92,0xA2,0xC2,/*Z*/
0x00,0x00,0xFE,0x82,0x00,/*[*/
0x80,0x60,0x10,0x0C,0x02,
0,0,0,0,0,
0x20,0x40,0x80,0x40,0x20,/*^*/
0x02,0x02,0x02,0x02,0x02,/*_*/
0x80,0x40,0x20,0x00,0x00,/*`*/
0x04,0x2A,0x2A,0x2A,0x1E,/*a*/
0xFE,0x12,0x22,0x22,0x1C,/*b*/
0x1C,0x22,0x22,0x22,0x14,/*c*/
0x1C,0x22,0x22,0x12,0xFE,/*d*/
0x1C,0x2A,0x2A,0x2A,0x18,/*e*/
0x10,0x7E,0x90,0x80,0x40,/*f*/
//0b00011000,0b00100101,0b00100101,0b00100101,0b00011110,/*g*/
0x10,0x2A,0x2A,0x2A,0x3C,/*g*/
0xFE,0x10,0x10,0x10,0x0E,/*h*/
0x00,0x12,0x5E,0x02,0x00,/*i*/
0x04,0x02,0x12,0x5C,0x00,/*j*/
0xFE,0x08,0x08,0x14,0x22,/*k*/
0x00,0x82,0xFE,0x02,0x00,/*l*/
0x3E,0x20,0x1C,0x20,0x1E,/*m*/
0x3E,0x20,0x20,0x20,0x1E,/*n*/
0x1C,0x22,0x22,0x22,0x1C,/*o*/
0x3F,0x24,0x24,0x24,0x18,/*p*/
0x18,0x24,0x24,0x3F,0x01,/*q*/
0x3E,0x10,0x20,0x20,0x10,/*r*/
0x12,0x2A,0x2A,0x2A,0x04,/*s*/
0x00,0x10,0x3C,0x12,0x04,/*t*/
0x3C,0x02,0x02,0x02,0x3E,/*u*/
0x30,0x0C,0x02,0x0C,0x30,/*v*/
0x38,0x06,0x18,0x06,0x38,/*w*/
0x22,0x14,0x08,0x14,0x22,/*x*/
0x30,0x0A,0x0A,0x0A,0x3C,/*y*/
0x22,0x26,0x2A,0x32,0x22,/*z*/
                             
};

char adc[] =  "  00oC";
char volt[] = "  0.0v";
char time[] = "  HH:MM - DDD NN 2011";

char days[7][3] ={
	"Sun","Mon","Tue","Wed","Thu","Fri","Sat"

};

char *str;
char x;
unsigned int prescale;
unsigned char a = 0,z = 0;

unsigned char Second,Minute,Hour,Day;

char flagTemp=1,flagUpdate,flagTime;
long IntDegC;

int main(void)
{
	BCSCTL1 =0;                        		// ACLK/2
	BCSCTL3 |= LFXT1S_2;                    // ACLK = VLO
	WDTCTL = WDTHOLD + WDTPW;                  // Set Watchdog Timer interval to ~30ms

	P1DIR = 0xFF;							// P1.6 output (green LED)
	P1OUT = 1;								// red LED on

	P2SEL = 0;
	P2DIR = 0;
	P2IE = P2IES = 0x40;
	P2IFG &= ~0x40;
	
	DCOCTL = 0x00;							// Set DCOCLK to 1MHz
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;

	CCR0 = 32;
	CCTL0 = CCIE;                             // CCR0 interrupt enabled
	CCR1 = 1024;
	CCTL1 = CCIE; 	
	TACTL = TASSEL_1 + ID_0 + MC_2 + TAIE;           // SMCLK/8, cont-mode


	USICTL0 |= USIPE6 + USIPE5 + USIMST + USIOE + USILSB; 	// Port, SPI master
	USICKCTL = USIDIV_0 + USISSEL_2 + USICKPL;      		// /2 SMCLK, == 250khz SPI Clock  
	USICTL0 &= ~USISWRST;                 					// USI released for operation


	__enable_interrupt();
	
//	str = &volt[0];
	str = &time[0];
	
	while(1)
	{
		// oC = ((A10/1024)*1500mV)-986mV)*1/3.55mV = A10*423/1024 - 278
		if(flagTemp)   
		{ 	
			
			ADC10CTL1 = INCH_10 + ADC10DIV_3;         // Temp Sensor ADC10CLK/4
			ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE;
			
			
			ADC10CTL0 |= ENC + ADC10SC;
			__bis_SR_register(LPM0_bits);
			
			int temp = ADC10MEM;
			int volts;
			IntDegC = ((temp - 673) * 423) / 1024;	
			
			ADC10CTL1 = INCH_11;                     // AVcc/2
			ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + ADC10IE + REF2_5V;
		     // __delay_cycles(240);
			
		    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		  
		    __bis_SR_register(LPM0_bits);        // LPM0 with interrupts enabled
		   
		 	temp = ADC10MEM;                  // Retrieve result
		//	ADC10CTL0 &= ~ENC;

			if(temp <= 550)
			{
				ADC10CTL1 = INCH_11;                     // AVcc/2
				ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + ADC10IE;// + REF2_5V;
			   	
				
			    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
			    __bis_SR_register(LPM0_bits);        // LPM0 with interrupts enabled
			    temp = ADC10MEM;                  // Retrieve result

			    volts = (temp*15)/512;
			}
			else
			{
				 volts = (temp*25)/512;
		 	}
		      /* Stop and turn off ADC */
		    ADC10CTL0 &= ~ENC;
			ADC10CTL0 &= ~(REFON + ADC10ON);
			
			volt[2] = volts/10 + '0';
			volt[4] = volts%10 + '0';

			adc[2] = IntDegC/10%10+'0';
			adc[3] = IntDegC%10+'0';
			
			flagTemp = 0;
			
			
		}
		
		
		if(flagUpdate)	
		{
			//======scrolling code, displays data stored in str
			char t,y;
			for(t = 0; t< sizeof(data) ; t++)
				data[t] = 0;
			
			for(y = 0; y< 5; y++)
			{
				load(str[(y+z)%(6)],y*8+a);
			}

			if(a == 0)
			{
				z++;
				a = 7;
				if(str[z] == 0)
					z = 0;		
			}
			else
				a--;
			
			flagUpdate = 0;
		}
		
		if(flagTime)
		{
			if(Second++ > 59)
			{
				Second = 0;
				if(Minute++ > 59)
				{
					Minute = 0;
					if(Hour++ > 23)
					{
						Hour = 0;
						if(Day++ > 6)
							Day = 0;
					}	
				}
			}
			
			time[2] = Hour/10;
			time[3] = Hour%10;
			
			time[5] = Minute/10;
			time[6] = Minute%10;
			
			time[9] = days[Day][0];
			time[10] = days[Day][1];
			time[11] = days[Day][2];
			
			
		}
		__bis_SR_register(LPM3_bits);             // Enter LPM3
		__no_operation();
	}
}

void load(int ascii,char address)
{
	char y;
	for(y = 0;y<5;y++)
		data[y+address] = font_data[(ascii-45)*5 +y];
}

const char offset[4] = {5,13,21,29};
// TimerA2 CCR0 Interrupt
__attribute__((interrupt(TIMERA0_VECTOR))) TimerA0(void)
{

	//CCTL0 &= ~CCIFG;  
	CCR0 += 32;

	P1OUT &= ~(0x1F);
	
	char t,d;
	for(t = 0; t < 4; t++)
	{
		d = data[x+offset[t]]>>1;
		while((USICTL1 & USIIFG) == 0);
		USICTL1 &= ~(1);
		USISRL = d;
		USICNT = 7;
	}
	
  	P1OUT |= (1<<x);
  
	if((x++) >= 5)
    	x = 0;
}


// TimerA2 CCR1/Overflow Interrupt
__attribute__((interrupt(TIMERA1_VECTOR))) TimerA1(void)
{
  
  	switch( TAIV )
  	{
  		case  2:                                  // CCR1
			CCR1 += 512;
			flagUpdate = 1;
			if(prescale++ > 10)
			{
				prescale = 0;
				flagTime = 1;
			}
     		break;
  		case  4: break;		                     // CCR2 Not avaliable
  		case 10:                          		 // overflow not used	
			flagTemp = 1;             			 // Sampling and conversion start
			break;
 	}

	__bic_SR_register_on_exit(LPM3_bits);
}

__attribute__((interrupt(ADC10_VECTOR))) adc10(void)
{
	__bic_SR_register_on_exit(LPM0_bits);
}



__attribute__((interrupt(PORT2_VECTOR))) P2(void)
{
	if(str == &adc[0])
		str = &volt[0];
	else
		str = &adc[0];
	P2IFG &= ~0x40;
}