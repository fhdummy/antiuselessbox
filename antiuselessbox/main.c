/*
 * antiuselessbox.c
 *
 * Created: 01.12.2015 17:26:00
 * Author : Stefan
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "KS0108.h"

#define PING_PIN PD2
#define PING_DDR DDRD
#define PING_OUTPUT_PORT PORTD
#define PING_INPUT_PORT PIND

double distance = 0;
int countGlobal = 0;

void initUSART()
{
	UBRRH = 0;
	UBRRL = 51;
	
	UCSRB = ((1<<TXEN));
	
	UCSRC = ((1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1));	//Asynchron 8N1
	UCSRB |= (1 << UDRIE);	//no parity, UMSEL for async operation
}

void initTimer1PWM()
{
	/* FastPWM, TOP: OCR1A, Update of OCR1X: BOTTOM, TOV1 Flag Set on: TOP, Prescaler 8 */
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11);
	
	TCCR1A |= ((1 << COM1A1) | (1 << COM1A0));	// Inverted mode
	
	ICR1 = 19999;	
}

void usartTransmit(unsigned char data)
{
	while(!(UCSRA & (1 << UDRE)))
	{
		
	}
	UDR = data;
}

void executePing()
{
	PING_DDR |= (1 << PING_PIN);
	PING_OUTPUT_PORT |= (1 << PING_PIN);	//Set pin high
	_delay_ms(5);
	PING_OUTPUT_PORT &= ~(1 << PING_PIN);	//Set pin low
	PING_DDR &= ~(1 << PING_PIN);	//Set pin to input
	
	while(!(PING_INPUT_PORT & (1 << PING_PIN)))	//Wait until the pin is high
	{
		
	}
	startTimer0();
	
	while(PING_INPUT_PORT & (1 << PING_PIN))	//Wait until the pin is low again
	{
		
	}
	stopTimer0();
	distance = TCNT0 * 0.55168;	//Value in microseconds
	TCNT0 = 0;
}

void startTimer0()
{
	TCCR0 &= ~((1 << WGM01) | (1 << WGM00));	//Normal mode
	TCCR0 &= ~((1 << COM01) | (1 << COM00));	//Normal port operation
	TCCR0 |= ((1 << CS02));	//Prescaler 256
	TCNT0 = 0;
}

void stopTimer0()
{
	TCCR0 &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
}

void initTimer2()
{
	TCCR2 &= ~((1 << WGM21) | (1 << WGM20));	//Normal mode
	TCCR2 &= ~((1 << COM21) | (1 << COM20));	//Normal port operation
	
	TCCR2 |= ((1 << CS22) | (1 << CS21) | (1 << CS20));
	
	TIMSK |= (1 << TOIE2);	//Timer 2 Overflow Interrupt Enable
}

int main(void)	
{
	
	GLCD_Initalize();
	
	GLCD_ClearScreen();
	GLCD_GoTo(0,0);
	GLCD_WriteString(" #   #######  ######");
	GLCD_GoTo(0,1);
	GLCD_WriteString(" #      #     #     ");
	GLCD_GoTo(0,2);
	GLCD_WriteString(" #      #     #     ");
	GLCD_GoTo(0,3);
	GLCD_WriteString(" #      #     #_____");
	GLCD_GoTo(0,4);
	GLCD_WriteString(" #      #          #");
	GLCD_GoTo(0,5);
	GLCD_WriteString(" #      #          #");
	GLCD_GoTo(0,6);
	GLCD_WriteString(" #      #          #");
	GLCD_GoTo(0,7);
	GLCD_WriteString(" #      #     ######");
	
	char distanceString[10];
	
	unsigned char enableServo = 0;
	unsigned char toggleDirection = 0;
	
	initUSART();
	initTimer2();
	//sei();
	initTimer1PWM();
	
	DDRD |= (1 << PD5);
	
    /* Replace with your application code */
    while (1) 
    {
		/*OCR1A = ICR1 - 800;
		_delay_ms(2000);
		OCR1A = ICR1 - 1600;
		_delay_ms(2000);*/
		
		sprintf(distanceString, "%3d", (int)distance);
		
		executePing();
		
		GLCD_GoTo(15,4);
		GLCD_WriteString(distanceString);
		
		if(distance <= 15)
		{
			enableServo = 1;
			usartTransmit('A');
		}
		
		if(enableServo == 1)
		{
			if(countGlobal >= 20)
			{
				if(toggleDirection == 0) 
				{
					OCR1A = ICR1 - 800;
					toggleDirection = 1;
				}				
				else if(toggleDirection == 1) 
				{
					OCR1A = ICR1 - 1600;
					toggleDirection = 0;
				}
				
				countGlobal = 0;
			}
			
			enableServo = 0;
		}
		
		_delay_ms(50);
		countGlobal++;
    }
	
	return 0;
}