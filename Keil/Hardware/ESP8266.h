#ifndef __ESP8266_H
#define __ESP8266_H

#include <stdint.h>

#define		WIFI_NAME				"YOUR_WIFI_NAME"
#define		WIFI_PASSWORD			"YOUR_WIFI_PASSWORD"
#define		USER_NAME				"YOUR_USER_NAME"
#define		USER_PASSWORD			"YOUR_USER_PASSWORD"
#define		CLIENTID				"YOUR_CLIENTID"
#define		MQTT_HOST				"MQTT_HOST"
#define		MQTT_PORT				"MQTT_PORT"
#define		MQTT_SUB_TOPIC			"YOUR_MQTT_SUB_TOPIC"
#define		MQTT_PUB_TOPIC			"YOUR_MQTT_PUB_TOPIC"


void ESP8266_ConnectAliyun(void);

#endif
