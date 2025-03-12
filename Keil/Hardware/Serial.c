#include "stm32f10x.h"                  // Device header
#include <stdio.h>

char Serial_RxPacket[100];				
uint8_t Serial_RxFlag = 0;  

void Serial_Init(uint32_t baudrate) {
	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   
    
	// PA9	USART1 TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA10 USART1 RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
	
	//中断输出配置
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//NVIC中断分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
 
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}

void Serial_SendData(uint8_t *data, uint16_t len) {
	uint16_t i;
    for (i = 0; i < len; i++) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, data[i]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
}

//void Serial_ReceiveData(uint8_t *buffer, uint16_t len) {
//	uint16_t i;
//    for (i = 0; i < len; i++) {
//        while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
//        buffer[i] = USART_ReceiveData(USART1);
//    }
//}

void Serial_SendString(const char *str) {
    while (*str) {
        Serial_SendData((uint8_t *)str, 1);
        str++;
    }
}

//void Serial_ReceiveString(char *buffer, uint16_t buffer_size) {
//    uint16_t i = 0;
//    char received_char;

//    do {
//        while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

//        received_char = USART_ReceiveData(USART1);
//        buffer[i++] = received_char;
//    } while (received_char != '\n' && i < buffer_size - 1);

//    buffer[i] = '\0';
//}

void USART1_IRQHandler(void) {	
	static uint8_t pRxPacket = 0;	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        uint8_t RxData = USART_ReceiveData(USART1);
        Serial_RxPacket[pRxPacket++] = RxData;
        
        if (RxData == '\n') {  
            Serial_RxPacket[pRxPacket] = '\0';
            Serial_RxFlag = 1;
            pRxPacket = 0;
        }
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
