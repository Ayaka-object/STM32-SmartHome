#include "stm32f10x.h" 

void LED_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_11 | GPIO_Pin_12);
}

void LED1_ON(void){
	GPIO_SetBits(GPIOB,GPIO_Pin_11);
}

void LED1_OFF(void){
	GPIO_ResetBits(GPIOB,GPIO_Pin_11);
}

void LED1_Turn(void){
	if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_11) == 0 ){
		GPIO_SetBits(GPIOB,GPIO_Pin_11);
	}else{
		GPIO_ResetBits(GPIOB,GPIO_Pin_11);
	}
}


void LED2_ON(void){
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
}

void LED2_OFF(void){
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}
void LED2_Turn(void){
	if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_12) == 0 ){
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
	}else{
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	}
}
