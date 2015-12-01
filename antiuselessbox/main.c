/*
 * antiuselessbox.c
 *
 * Created: 01.12.2015 17:26:00
 * Author : Stefan
 */ 

#include <avr/io.h>
#include <util/delay.h>

#define PING_PIN PA0
#define PING_DDR DDRA
#define PING_OUTPUT_PORT PORTA
#define PING_INPUT_PORT PINA

double distance = 0;

void initUSART()
{
	UBRRH = 0;
	UBRRL = 51;
	
	UCSRB = ((1<<TXEN));
	
	UCSRC = ((1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1));	//Asynchron 8N1
	UCSRB |= (1 << UDRIE);	//no parity, UMSEL for async operation
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
	startTimer1();
	
	while(PING_INPUT_PORT & (1 << PING_PIN))	//Wait until the pin is low again
	{
		
	}
	stopTimer1();
	distance = TCNT1 * 0.1724;	//Value in microseconds
	TCNT1 = 0;
}

void startTimer1()
{
	//Normal-Mode
	TCCR1A &= ~((1 << WGM10) | (1 << WGM11));
	TCCR1B &= ~((1 << WGM12) | (1 << WGM13));
		
	//Prescaler 64
	TCCR1B |= ((1 << CS10) | (1 << CS11));
	TCCR1B &= ~((1 << CS12));
		
	TCNT1 = 0;
}

void stopTimer1()
{
	TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));	//Stop the timer
	//TCNT1 = 0;	
}

int main(void)	
{
	initUSART();
	
    /* Replace with your application code */
    while (1) 
    {
		executePing();
		
		if(distance <= 30)
		{
			usartTransmit('A');
		}
		
		_delay_ms(50);
    }
}

