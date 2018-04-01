

#include<reg51.h>
#include<stdio.h>
#include<string.h>
#include <stdlib.h>
sbit DHT11=P3^1;		/* Connect DHT11 Sensor Pin to P2.1 Pin */
int I_RH,D_RH,I_Temp,D_Temp,CheckSum; 

void timer_delay20ms()		/* Timer0 delay function */
{
	TMOD = 0x10;
	TH1 = 0xB8;				/* Load higher 8-bit in TH0 */
	TL1 = 0x0C;				/* Load lower 8-bit in TL0 */
	TR1 = 1;					/* Start timer0 */
	while(TF1 == 0);	/* Wait until timer0 flag set */
	TR1 = 0;					/* Stop timer0 */
	TF1 = 0;					/* Clear timer0 flag */
}

void timer_delay30us() /* Timer0 delay function */
{
	TMOD = 0x10;			/* Timer0 mode1 (16-bit timer mode) */
	TH1 = 0xFF;				/* Load higher 8-bit in TH0 */
	TL1 = 0xF1;				/* Load lower 8-bit in TL0 */
	TR1 = 1;					/* Start timer0 */
	while(TF1 == 0);	/* Wait until timer0 flag set */
	TR1 = 0;					/* Stop timer0 */
	TF1 = 0;					/* Clear timer0 flag */
}

void Request()			/* Microcontroller send start pulse or request */
{
	DHT11 = 0;		 		/* set to low pin */
	timer_delay20ms();/* wait for 20ms */
	DHT11 = 1;				/* set to high pin */
}

void Response()			/* receive response from DHT11 */
{
	while(DHT11==1);
	while(DHT11==0);
	while(DHT11==1);
}

int Receive_data()			/* receive data */
{
	int q,c=0;	
	for (q=0; q<8; q++)
	{
		while(DHT11==0);		/* check received bit 0 or 1 */
		timer_delay30us();
		if(DHT11 == 1)			/* if high pulse is greater than 30ms */
		c = (c<<1)|(0x01);	/* then its logic HIGH */
		
		else								/* otherwise its logic LOW */
		c = (c<<1);
		
		while(DHT11==1);
	}
	return c;
}

int getTemp()
{
	
	//LCD_Init();								/* initialize LCD */
	
while(1)
	{		
		Request();							/* send start pulse */
		Response();							/* receive response */
		
		I_RH=Receive_data();		/* store first eight bit in I_RH */		
		D_RH=Receive_data();		/* store next eight bit in D_RH */	
		I_Temp=Receive_data();	/* store next eight bit in I_Temp */
		D_Temp=Receive_data();	/* store next eight bit in D_Temp */
		CheckSum=Receive_data();/* store next eight bit in CheckSum */

		if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
		{
			continue;
		}
		
		else
		{	
			break;
		}		
		timer_delay20ms();
	}	
	
	return I_Temp;
}

