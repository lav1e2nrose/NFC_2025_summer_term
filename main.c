#include <reg52.h>
#include "uart.h"
#include "bluetooth.h"
#include "AT24C02.h"
#include "oled.h"
#include "ui.h"
#include "utils.h"

// 定义按键
sbit KEY1 = P3^0;  // 发送按键
sbit KEY2 = P3^1;  // 接收按键

// 全局变量
CARD receivedCard;   // 接收到的卡片数据

/**
 * @brief 检测按键
 * @return 按键值：1表示KEY1，2表示KEY2，0表示无按键
 */
u8 KEY_Scan(void)
{
    if (KEY1 == 0)
    {
        delay_ms(10);  // 消抖
        if (KEY1 == 0)
        {
            while (KEY1 == 0);  // 等待按键释放
            return 1;
        }
    }
    
    if (KEY2 == 0)
    {
        delay_ms(10);  // 消抖
        if (KEY2 == 0)
        {
            while (KEY2 == 0);  // 等待按键释放
            return 2;
        }
    }
    
    return 0;  // 无按键按下
}

/**
 * @brief 主函数
 */
void main(void)
{
    u8 key;
    
    // 初始化各个模块
    UI_Init();          // 初始化用户界面
    Bluetooth_Init();   // 初始化蓝牙
    
    // 进入主循环
    while (1)
    {
        // 检测按键
        key = KEY_Scan();
        
        if (key == 1)  // 按下KEY1，发送卡片数据
        {
            UI_Update(UI_SENDING);
            Bluetooth_SendAllCards();  // 发送所有卡片数据
            UI_Update(UI_IDLE);
        }
        
        // 处理蓝牙接收
        Bluetooth_Process();
        
        // 根据蓝牙状态更新UI
        if (Bluetooth_GetStatus() == BT_SENDING)
        {
            UI_Update(UI_SENDING);
        }
        else if (Bluetooth_GetStatus() == BT_FINISHED)
        {
            // 接收完成，显示接收到的数据
            UI_ShowReceived(receivedCard);
        }
        else if (Bluetooth_GetStatus() == BT_IDLE)
        {
            UI_Update(UI_IDLE);
        }
    }
}