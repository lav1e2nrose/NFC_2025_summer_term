#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "uart.h"
#include "AT24C02.h"

// 蓝牙状态定义
#define BT_IDLE     0   // 空闲状态
#define BT_SENDING  1   // 正在发送
#define BT_RECEIVING 2  // 正在接收
#define BT_FINISHED 3   // 传输完成

// 函数声明
void Bluetooth_Init(void);                      // 蓝牙初始化
void Bluetooth_SendCard(CARD card);             // 发送卡片数据
void Bluetooth_SendAllCards(void);              // 发送所有卡片数据
u8 Bluetooth_GetStatus(void);                   // 获取当前蓝牙状态
void Bluetooth_Process(void);                   // 蓝牙处理函数，在主循环中调用
void Bluetooth_ReceiveData(void);               // 接收数据

#endif