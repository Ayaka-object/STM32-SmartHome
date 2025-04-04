# 基于STM32的智能家具/STM32-SmartHome

**项目概述：**

这是一个简单的基于stm32的智能家具项目。本项目设计并实现了一个基于 STM32 的智能家具控制系统。STM32F103C8T6 作为主控芯片，通过串口与 ESP8266 模块进行通讯，
ESP8266 使用 MQTT 协议通过 AT 指令连接至阿里云服务器，从而实现远程数据传输。该系统配备了多种外设，包括 DHT11 温湿度传感器、直流电机和 LED 灯，支持用户通
过Android 手机应用进行远程控制。手机应用可实时显示温湿度数据，同时用户可以远程控制 LED 灯的开关以及风扇（电机）的转动，极大提升了智能家居的便捷性和可操作性。


**主要功能：**

-**温湿度监测**：实时获取 DHT11 温湿度传感器的数据，并在手机应用上显示。

-**远程控制**：通过手机app可实现对LED和风扇等外设的远程控制

-**屏幕显示**：OLED显示屏也可显示温湿度数据以及显示LED和风扇（电机）的工作状态



**物料清单：**

-**主控芯片**：STM32F103C8T6

-**WiFi模块**：ESP8266

-**传感器**：DHT11

-**外设**：LED，电机驱动模块，直流电机，0.96寸OLED显示屏


**接线：**

PA1---DHT11（DO）

PA9---ESP8266（RX）

PA10---ESP8266（TX）

PB11---LED（+）

-电机及OLED接线

![6-5 PWM驱动直流电机](https://github.com/user-attachments/assets/530ec76b-8c3d-40e9-a472-111e3ca4716d)


**项目演示：**

准备中。。。
