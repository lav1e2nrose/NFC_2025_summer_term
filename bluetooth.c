#include "bluetooth.h"
#include "AT24C02.h"

// 蓝牙模块初始化
void BT_Init(void)
{
    // 设置蓝牙通信引脚初始状态
    BT_TX = 1;  // 空闲状态为高电平
    
    // 蓝牙模块一般在上电后自动进入AT命令模式或通信模式
    // 此处可以通过发送一些AT命令来配置蓝牙模块
    delay_ms(500);  // 等待蓝牙模块稳定
}

// 发送一个字节数据（软件模拟UART发送）
void BT_SendByte(unsigned char dat)
{
    unsigned char i;
    
    // 发送起始位
    BT_TX = 0;
    delay_ms(1);  // 延时约9600bps的一位时间，使用delay_ms代替delay_us
    
    // 发送8位数据位
    for(i = 0; i < 8; i++)
    {
        BT_TX = (dat & 0x01) ? 1 : 0;  // 从低位开始发送
        dat >>= 1;
        delay_ms(1);
    }
    
    // 发送停止位
    BT_TX = 1;
    delay_ms(1);
}

// 发送字符串
void BT_SendString(unsigned char *str)
{
    while(*str)
    {
        BT_SendByte(*str++);
    }
}

// 发送卡信息到蓝牙
void BT_SendCardInfo(void)
{
    unsigned char i;
    CARD card;
    
    BT_SendString("Card Info in EEPROM:\r\n");
    
    // 从EEPROM读取所有卡片信息并通过蓝牙发送
    for(i = 0; i < 3; i++)
    {
        AT24C02_ReadCard_pos(i, &card);
        
        // 发送卡片序号
        BT_SendString("Card ");
        BT_SendByte('1' + i);
        BT_SendString(": ");
        
        // 发送卡ID
        BT_SendString("ID=");
        BT_SendString(card.id);
        BT_SendString(", ");
        
        // 发送学号
        BT_SendString("Student ID=");
        BT_SendString(card.stuid);
        BT_SendString("\r\n");
        
        delay_ms(100);  // 短暂延时，避免数据发送太快
    }
    
    BT_SendString("Transfer complete.\r\n");
}