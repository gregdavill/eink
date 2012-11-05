#include <msp430.h>
#include "eink.h"
#include "delay.h"
#include "spi.h"
#include "main.h"

//	Display Buffer
unsigned char display_data[20];


unsigned char displayInverse;


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

const unsigned int font_data[]// ={
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

void einkTx(int f,int b,int loopcnt);


void einkPuts(const char* str,char l)
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

void einkUpdate(void){
	
	P1OUT |= SLEEPBpin; // enable screen updates

	if(displayInverse == 1)
	{
		einkTx(0xffff,3,6);
		einkTx(0,3,5);
		einkTx(0xffff,3,6);
	}
	else
	{
		einkTx(0,1,7);
	}


	einkTx(0,1,8);

	einkTx(0xFFFF,1,8);	

	P1OUT &= ~SLEEPBpin; // lock screen RAM

}


void einkClearData(void)
{
	int i;
	for(i = 0;i< 20; i++)
	{
		display_data[i] = 0;
	}
}


void einkTx(int f,int b,int loopcnt)
{
	
	int t,y,loops;
	

	if(b != 1)
	f = displayInverse == 0 ? f : ~f;

	modeMSB = 1; 	// MSB first (flips bottom row characters)

	for(y = 0; y < 2; y++)
	{
		P1OUT &= ~EIO1pin;
		
		spiTx(displayInverse == 0 ? 0xFFFF : 0,1);
		
		P1OUT |= EIO1pin;
		
		for(t = 0; t < 10; t++)
		{
			int d;
			int e = display_data[t+(10*y)];
		
			if(b == 1)
			{
				d = ~font_data[e];
				if(displayInverse)
					d = ~d;
			}
			else if(b == 2)
			{
				d = font_data[e];
				if(displayInverse)
					d = ~d;
			}
			else if(b == 3)
				d = 0;
			else
				d = 0xFFFF;
			
			spiTx(d, 16);
		}
		spiTx(f,1);
		
		modeMSB = 0; 	// LSB first (top data not flipped)
	}

	// loop the latch pin, redrives display each time.
	for(loops = 0; loops < loopcnt; loops++)
	{
		P1OUT |= LATCHpin;
		P1OUT &= ~LATCHpin;

		delay(128);
	}

}