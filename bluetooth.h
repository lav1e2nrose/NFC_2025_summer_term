#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include <reg52.h>
#include "utils.h"

// 定义蓝牙通信引脚
sbit BT_TX = P2^0;  // 软件模拟的发送引脚
sbit BT_RX = P2^1;  // 软件模拟的接收引脚

// 蓝牙初始化函数
void BT_Init(void);

// 发送一个字节
void BT_SendByte(unsigned char dat);

// 发送字符串
void BT_SendString(unsigned char *str);

// 发送卡信息到蓝牙
void BT_SendCardInfo(void);

#endif