#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "ESP8266.h"
#include "Serial.h"
#include "LED.h"
#include "Motor.h"
#include "DHT11.h"
#include "Timer.h"
#include <stdio.h>
#include <string.h>

extern const uint8_t Special_Font[][32];
extern const uint8_t Week_Font[][32];
extern unsigned int rec_data[4];
char json_data[100];

char mqtt_message[256];
char time_message[256];

volatile uint8_t Timer_Flag = 0;
volatile uint8_t Datetime_Flag = 0;

//DHT11数据刷新达到一定次数时，上传刷新一次APP温湿度数据
uint8_t send_DHT11_Data_count = 0;

uint8_t i;

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
	int week;
} DateTime;

const char *month_table[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

const char *week_table[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

void Process_Serial_Data(void){
	if (Serial_RxFlag == 1)	{
		Serial_RxFlag = 0;
		//判断接收到的是什么类型的消息，便于进一步处理
		if (strstr((char*)Serial_RxPacket, "+MQTTSUBRECV") != NULL) {
            strcpy(mqtt_message, (char*)Serial_RxPacket);	//MQTT类型数据
        } else if(strstr((char*)Serial_RxPacket, "+CIPSNTPTIME") != NULL){
			strcpy(time_message, (char*)Serial_RxPacket);
		}
	}
}

void Process_MQTT_Message(void){
	if (strlen(mqtt_message) > 0){
		if (strstr((char*)Serial_RxPacket, "LED_ON") != NULL){
			OLED_ShowString(4, 1, "                ");
			OLED_ShowString(4, 1, "LED_ON_OK");
			LED1_ON();
		}else if(strstr((char*)Serial_RxPacket, "LED_OFF") != NULL){
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
		memset(mqtt_message, 0, sizeof(mqtt_message));
	}
}

void DHT11_Update(void){
	if(Timer_Flag == 1){
		Timer_Flag = 0;
		if(DHT11_ReadData() == SUCCESS){
			//湿度
			OLED_ShowNum(3, 7, rec_data[0], 2);
			OLED_ShowString(3, 9, ".");
			OLED_ShowNum(3, 10, rec_data[1], 2);
			
			//温度
			OLED_ShowNum(2, 7, rec_data[2], 2);
			OLED_ShowString(2, 9, ".");
			OLED_ShowNum(2, 10, rec_data[3], 1);
			
			send_DHT11_Data_count++;
			if(send_DHT11_Data_count == 3){
				send_DHT11_Data_count = 0;
				sprintf(json_data, "AT+MQTTPUB=0,\""MQTT_PUB_TOPIC"\",\"{\\\"temp\\\":{\\\"value\\\":%d}\\,\\\"humidity\\\":{\\\"value\\\":%d}\\}\",0,0\r\n",rec_data[2], rec_data[0]);
				Serial_SendString(json_data);
			}
		}
	}
}

//时间解析处理函数
int GetMonthNumber(const char *monthStr) {
    for (int i = 0; i < 12; i++) {
        if (strcmp(monthStr, month_table[i]) == 0) {
            return i + 1;
        }
    }
    return -1;
}

int GetWeekNumber(const char *weekStr) {
    for (int i = 0; i < 7; i++) {
        if (strcmp(weekStr, week_table[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int Parse_ESP8266_Time(const char *timeStr, DateTime *dt) {
    char week[4], month[4];
    int day, year, hour, minute, second;

    // 解析esp8266返回的时间数据
    if (sscanf(timeStr, "+CIPSNTPTIME:%3s %3s %d %d:%d:%d %d",
               week, month, &day, &hour, &minute, &second, &year) != 7) {
        return 0; // 解析失败
    }
			   
	if (year == 1970) {
        return 0; // 获得了错误的时间，不进行更新
    }

    dt->year = year;
    dt->month = GetMonthNumber(month);
    dt->day = day;
    dt->hour = hour;
    dt->minute = minute;
    dt->second = second;
	dt->week = GetWeekNumber(week);
    return (dt->month != -1 ); // 月份有效，解析成功
}

void Datetime_Update(void){
	if(Datetime_Flag == 1){
		Datetime_Flag = 0;
		//发送AT指令获取时间
		Delay_ms(1000);
		Serial_SendString("AT+CIPSNTPTIME?\r\n");
		//解析并显示时间
		DateTime dt;
		if (strlen(time_message) > 0){
			if (Parse_ESP8266_Time(time_message, &dt)){
				
				OLED_ShowNum(1, 1, dt.month, 2);
				OLED_ShowNum(1, 4, dt.day, 2);
				
				OLED_ShowNum(1, 7, dt.hour, 2);
				OLED_ShowNum(1, 10, dt.minute, 2);
				
				OLED_ShowChinese(112, 0, dt.week, Week_Font);
			}
		}
	}
}

void OLED_Init_Display(){
	//第一行 显示日期 时间 周几
	OLED_ShowString(1, 3, "/");
	OLED_ShowString(1, 9, ":");
	OLED_ShowChinese(96, 0, 6, Special_Font);
	//第二行 显示温度
	OLED_ShowChinese(0, 2, 1, Special_Font);
	OLED_ShowChinese(16, 2, 3, Special_Font);
	OLED_ShowString(2, 5, ":");
	OLED_ShowChinese(84, 2, 0, Special_Font);
	//第三行 显示湿度
	OLED_ShowChinese(0, 4, 2, Special_Font);
	OLED_ShowChinese(16, 4, 3, Special_Font);
	OLED_ShowString(3, 5, ":");
	OLED_ShowChinese(92, 4, 7, Special_Font);
}

int main(void){
	
	LED_Init();
    OLED_Init();
	Motor_Init();
	Timer_Init();
	Serial_Init(115200); 
	ESP8266_ConnectAliyun();
	OLED_Init_Display();
	
	while(1){
		//对串口接收消息进行分类
		Process_Serial_Data();
		//处理MQTT类型消息
		Process_MQTT_Message();
		//更新日期数据
		Datetime_Update();
		//更新DHT11数据
		DHT11_Update();
		
    }
}

void TIM3_IRQHandler(void){
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET){
		//触发中断时，立即刷新一次DHT11的值
		Timer_Flag = 1;
		i++;
		if(i == 10){
			i = 0;
			Datetime_Flag = 1;
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
