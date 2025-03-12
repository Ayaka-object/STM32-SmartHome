#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "ESP8266.h"
#include "Serial.h"
#include "LED.h"
#include "Motor.h"
#include "DHT11.h"
#include <stdio.h>
#include <string.h>

extern unsigned int rec_data[4];
char json_data[100];

int main(void){
	
	LED_Init();
    OLED_Init();
	Motor_Init();
	Serial_Init(115200); 
	ESP8266_ConnectAliyun();
	//防止esp8266返回值对命令判断造成干扰
	Serial_RxFlag = 0;
	
	while(1){
		OLED_ShowString(1, 1, "temp:");
		OLED_ShowString(2, 1, "humidity:");
		if(DHT11_ReadData() == SUCCESS){
			OLED_ShowNum(2, 10, rec_data[0], 2);
			OLED_ShowString(2, 12, ".");
			OLED_ShowNum(2, 13, rec_data[1], 2);
			
			OLED_ShowNum(1, 6, rec_data[2], 2);
			OLED_ShowString(1, 8, ".");
			OLED_ShowNum(1, 9, rec_data[3], 1);
		
			sprintf(json_data, "AT+MQTTPUB=0,\"topic\",\"{\\\"temp\\\":{\\\"value\\\":%d}\\,\\\"humidity\\\":{\\\"value\\\":%d}\\}\",0,0\r\n",rec_data[2], rec_data[0]);
			Serial_SendString(json_data);
		}
		
		if (Serial_RxFlag == 1)		
		{			
			if (strstr((char*)Serial_RxPacket, "LED_ON") != NULL){
				OLED_ShowString(4, 1, "                ");
				OLED_ShowString(4, 1, "LED_ON_OK");
				LED1_ON();
			}else if (strstr((char*)Serial_RxPacket, "LED_OFF") != NULL){
				OLED_ShowString(4, 1, "                ");
				OLED_ShowString(4, 1, "LED_OFF_OK");
				LED1_OFF();	
			}else if (strstr((char*)Serial_RxPacket, "Motor_ON") != NULL){
				OLED_ShowString(4, 1, "                ");
				OLED_ShowString(4, 1, "Motor_ON_OK");
				Motor_ON();
			}else if (strstr((char*)Serial_RxPacket, "Motor_OFF") != NULL){
				OLED_ShowString(4, 1, "                ");
				OLED_ShowString(4, 1, "Motor_OFF_OK");
				Motor_OFF();
			}
			memset(Serial_RxPacket, 0, 100);
			Serial_RxFlag = 0;			
		}
    }
}
