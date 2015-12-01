/**
* @file CircularBuffer.c
* @brief CircularBuffer functions
*
* This module defines the functions for the circular buffer.
*
*/


#include <avr/io.h>
#include "CircularBuffer.h"

/**
* @brief Pointer to the read element of the circular buffer.
*/
unsigned char *pRead;

/**
* @brief Pointer to the write element of the circular buffer.
*/
unsigned char *pWrite;

/**
* @brief Circular buffer of the size CIRCULARBUFFERSIZE, which is defined in CircularBuffer.h
*/
unsigned char circularBuffer[CIRCULARBUFFERSIZE];

/**
* @brief Puts a string into the circular buffer
*
* Takes the string and puts it into the buffer, when the buffer is big enough and there is enough space.
*
* int sendUartString(unsigned char *pData, int length)
* @param *pData Data which is to send
* @param length Length of data
* @return int Return code of the main program. Codes other than zero signals an error. Return code of 1 indicates, that the string is too long for the buffer.
*/
int sendUartString(unsigned char *pData, int length)
{
	int bufferFreeLength;
	
	if(pWrite >= pRead)
	{
		bufferFreeLength = CIRCULARBUFFERSIZE - (int)pWrite + (int)pRead - 1;
	}
	else
	{
		bufferFreeLength = (int)pRead - (int)pWrite - 1;
	}
	
	if(length > bufferFreeLength) return 1;
	
	for(int i = 0; i < length; i++)
	{
		*pWrite = pData[i];
		pWrite++;
		if(pWrite >= &circularBuffer[CIRCULARBUFFERSIZE]) pWrite = circularBuffer;
	}
	
	UCSRB |= (1 << UDRIE);
	
	return 0;
} 

/**
* @brief Initialize the circular buffer
*
* Sets pRead and pWrite to the beginning of the circular buffer.
*
* @return void
*/
void initCircularBuffer()
{
	pRead = circularBuffer;
	pWrite = circularBuffer;
}



