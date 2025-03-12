#ifndef __DHT11_H
#define __DHT11_H

void DHT11_Init_OUT(void);
void DHT11_Init_IN(void);
void DHT11_Start(void);
char DHT11_ReadByte(void);
uint8_t DHT11_ReadData(void);

#endif
