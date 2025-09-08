#ifndef __UI_H__
#define __UI_H__

#include "oled.h"

// 界面状态
#define UI_IDLE      0  // 空闲界面
#define UI_SENDING   1  // 发送界面
#define UI_RECEIVED  2  // 接收界面

// 函数声明
void UI_Init(void);                     // 初始化界面
void UI_Update(u8 status);              // 更新界面状态
void UI_ShowIdle(void);                 // 显示空闲界面
void UI_ShowSending(void);              // 显示发送界面
void UI_ShowReceived(CARD card);        // 显示接收到的数据

#endif