#ifndef __UART_H__
#define __UART_H__

#include <reg52.h>
#include "utils.h"

// 软件串口引脚定义
sbit UART_TX = P2^1;  // 发送引脚
sbit UART_RX = P2^0;  // 接收引脚

// 函数声明
void SoftUart_Init(void);       // 软件串口初始化
void SoftUart_SendByte(u8 dat); // 发送一个字节
void SoftUart_SendString(u8 *str); // 发送字符串
u8 SoftUart_ReceiveByte(void);  // 接收一个字节
bit SoftUart_DataReady(void);   // 检查是否有数据可以接收

#endif