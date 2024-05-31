/*
 * Main.c
 *
 *  Created on: Jan 31, 2021
 *      Author: Mohamed
 */
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#define CLEAR_BIT(REG,NUM) REG &= ~(1<<NUM)
#define SET_BIT(REG,NUM) REG |= (1<<NUM)
#define DISPLAY(NUM) PORTC = (PORTC & 0xF0) | ((NUM) & 0x0F)

/* GLOBAL VARIABLES */
unsigned char Stick=0 , Mtick=0 , Htick=0 ;

/* ISR Definition*/
ISR(TIMER1_COMPA_vect)
{
	Stick++;
	if (60 == Stick)
	{
		Stick = 0;
		Mtick++;
		if (60 == Mtick)
		{
			Mtick = 0;
			Htick++;
			if (24 == Htick)
			{
				Htick = 0;
			}
		}
	}
}

ISR(INT0_vect)
{
	TCNT1 = 0;
	Stick = Mtick = Htick = 0;
}

ISR(INT1_vect)
{
	CLEAR_BIT(TCCR1B,CS10);
	CLEAR_BIT(TCCR1B,CS11);
	CLEAR_BIT(TCCR1B,CS12);
}

ISR(INT2_vect)
{
	SET_BIT(TCCR1B,CS12);
	SET_BIT(TCCR1B,CS10);
}
void main(void)
{
	unsigned char i;
	/* Configuring pin 2 and 3 in port D and pin 2 in port B as input */
	CLEAR_BIT(DDRD,2);
	CLEAR_BIT(DDRD,3);
	CLEAR_BIT(DDRB,2);
	/* Configuring first 4 pins in port C as output */
	SET_BIT(DDRC,0);
	SET_BIT(DDRC,1);
	SET_BIT(DDRC,2);
	SET_BIT(DDRC,3);
	/* Configuring first 6 pins in port A as output */
	SET_BIT(DDRA,0);
	SET_BIT(DDRA,1);
	SET_BIT(DDRA,2);
	SET_BIT(DDRA,3);
	SET_BIT(DDRA,4);
	SET_BIT(DDRA,5);
	/* Internal Resistance Configuration */
	SET_BIT(PORTD,2);
	SET_BIT(PORTB,2);
	/* Configuring external interrupts */
	MCUCR = (MCUCR&0xf0) | (0x0e);
	MCUCSR &= ~(1<<ISC2);
	GICR |= 0xe0;
	/* Enabling I-Bit */
	SREG |= (1<<7);
	/* Timer Configuration */
	SET_BIT(TCCR1B,WGM12); //CTC Mode
	SET_BIT(TCCR1B,CS12);
	SET_BIT(TCCR1B,CS10);
	TCNT1 = 0;
	OCR1A = 977;
	SET_BIT(TIMSK,OCIE1A);
	while(1)
	{
		for (i = 0 ; i < 6 ; i++)
		{
			PORTA = (PORTA & 0xC0) | (0x3F & (1<<i));
			switch(i)
			{
			case 0 :
				DISPLAY(Htick/10);
				break;
			case 1 :
				DISPLAY(Htick%10);
				break;
			case 2 :
				DISPLAY(Mtick/10);
				break;
			case 3 :
				DISPLAY(Mtick%10);
				break;
			case 4 :
				DISPLAY(Stick/10);
				break;
			case 5 :
				DISPLAY(Stick%10);
				break;
			}
			_delay_ms(4);
		}
	}
}

