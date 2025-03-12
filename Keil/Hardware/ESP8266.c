#include "ESP8266.h"
#include "Serial.h"
#include "Delay.h"
#include <string.h>
#include <stdio.h>

void ESP8266_ConnectAliyun(void){
	Serial_SendString("AT+RST\r\n");
    Delay_ms(1000);
    Serial_SendString("AT+CWMODE=1\r\n");  
    Delay_ms(1000);
	Serial_SendString("AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"\r\n");  
    Delay_ms(1000);
	Serial_SendString("AT+CWJAP=\"WiFi名称\",\"WiFi密码\"\r\n");  
    Delay_ms(1000);
	Serial_SendString("AT+MQTTUSERCFG=0,1,\"NULL\",\"用户名\",\"密码\",0,0,\"\"\r\n");  
    Delay_ms(1000);
	Serial_SendString("AT+MQTTCLIENTID=0,\"ClienID\"\r\n");
    Delay_ms(1000);
	Serial_SendString("AT+MQTTCONN=0,\"连接域名\",1883,1\r\n");
    Delay_ms(1000);
	//订阅发布指令
	Serial_SendString("AT+MQTTSUB=0,\"topic\",1\r\n");
    Delay_ms(1000);
}


