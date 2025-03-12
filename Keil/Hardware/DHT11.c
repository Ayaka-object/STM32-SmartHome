#include "stm32f10x.h"                  // Device header
#include "Delay.h"

unsigned int rec_data[5];

void DHT11_Init_OUT(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void DHT11_Init_IN(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void DHT11_Start(void){
	DHT11_Init_OUT();
	
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	Delay_us(30);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	Delay_ms(20);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	Delay_us(30);
	
	DHT11_Init_IN();
}

char DHT11_ReadByte(void){
	uint16_t i;
	unsigned char Data;
	for(i = 0; i < 8; i++){
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0);
		Delay_us(30);
		
		Data <<= 1;
		//ÅÐ¶ÏÊý¾Ý 
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1){
			Data |= 1;
		}
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1);
	}
	return Data;
}

uint8_t DHT11_ReadData(void){	
	DHT11_Start();
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0){
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0);
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 1);
		
		rec_data[0] = DHT11_ReadByte();
		rec_data[1] = DHT11_ReadByte();
		rec_data[2] = DHT11_ReadByte();
		rec_data[3] = DHT11_ReadByte();
		rec_data[4] = DHT11_ReadByte();
		
		GPIO_ResetBits(GPIOC, GPIO_Pin_2);
		Delay_us(55);
		GPIO_SetBits(GPIOC, GPIO_Pin_2);
		
		if(rec_data[0] + rec_data[1] + rec_data[2] + rec_data[3] == rec_data[4]){
			return SUCCESS;
		}else{
			return ERROR;
		}
	}else{
		return ERROR;
	}
}


