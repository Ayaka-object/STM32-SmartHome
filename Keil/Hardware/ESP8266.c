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
	Serial_SendString("AT+CWJAP=\""WIFI_NAME"\",\""WIFI_PASSWORD"\"\r\n");  
    Delay_ms(2000);
	
	//配置MQTT连接
	Serial_SendString("AT+MQTTUSERCFG=0,1,\"NULL\",\""USER_NAME"\",\""USER_PASSWORD"\",0,0,\"\"\r\n");  
    Delay_ms(1000);
	Serial_SendString("AT+MQTTCLIENTID=0,\""CLIENTID"\"\r\n");
    Delay_ms(1000);
	Serial_SendString("AT+MQTTCONN=0,\""MQTT_HOST"\","MQTT_PORT",1\r\n");
    Delay_ms(1000);
	
	//订阅发布指令
	Serial_SendString("AT+MQTTSUB=0,\""MQTT_SUB_TOPIC"\",1\r\n");
	Delay_ms(1000);
}


