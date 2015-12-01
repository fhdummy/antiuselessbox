#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#define CIRCULARBUFFERSIZE 40

extern unsigned char *pRead;
extern unsigned char *pWrite;
extern unsigned char circularBuffer[CIRCULARBUFFERSIZE];

void initCircularBuffer();
int sendUartString(unsigned char *, int);

#endif 
