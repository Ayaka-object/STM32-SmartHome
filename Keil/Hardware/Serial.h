#ifndef __SERIAL_H
#define __SERIAL_H

extern char Serial_RxPacket[];
extern uint8_t Serial_RxFlag;

void Serial_Init(uint32_t baudrate);
void Serial_SendData(uint8_t *data, uint16_t len);
void Serial_SendString(const char *str);

#endif
