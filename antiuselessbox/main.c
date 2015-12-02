/*
 * antiuselessbox.c
 *
 * Created: 01.12.2015 17:26:00
 * Author : Stefan
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "KS0108.h"

/* DEFINES */
#define PING_PIN PD2
#define PING_DDR DDRD
#define PING_OUTPUT_PORT PORTD
#define PING_INPUT_PORT PIND
#define SERVO_PIN PD5

/* GLOBAL VARIABLES */
double distance = 0;
int countGlobal = 0;

/* PROTOTYPES */
void initUSART();
void initTimer1PWM();
void initServo();
void usartTransmit(unsigned char);
void executePing();
void startTimer0();
void stopTimer0();

/* MAIN FUNCTION */
int main(void)	
{
	/* Variables */
	char distanceString[10];
	unsigned char enableServo = 0;
	unsigned char toggleDirection = 0;
	
	_delay_ms(50);
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
	
	initUSART();
	initTimer1PWM();
	initServo();
	
    while (1) 
    {
		sprintf(distanceString, "%3d", (int)distance);	//Convert integer to char array
		
		executePing();	//Make distant measurement
		
		/* Write distance to GLCD */
		GLCD_GoTo(15,3);
		GLCD_WriteString(distanceString);
		
		if(distance <= 15)
		{
			enableServo = 1;
			usartTransmit('A');
		}
		
		if(enableServo == 1)
		{
			if(countGlobal >= 20)	//Only execute this function every seconds
			{
				if(toggleDirection == 0) 
				{
					OCR1A = ICR1 - 800;	//Position 1 of the servo
					toggleDirection = 1;
				}				
				else if(toggleDirection == 1) 
				{
					OCR1A = ICR1 - 1600;	//Position 2 of the servo
					toggleDirection = 0;
				}
				
				countGlobal = 0;
			}
			
			enableServo = 0;
		}
		
		_delay_ms(50);
		
		countGlobal++;	//Increment count variable
    }
	
	return 0;
}

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

void initServo()
{
	DDRD |= (1 << SERVO_PIN);
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
	distance = TCNT0 * 0.55168;	//Value in centimeter
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