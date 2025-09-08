#include "uart.h"

// 波特率为9600，晶振11.0592MHz
// 定时器设置值
#define TIMER_VALUE (65536 - 11059200/12/9600)

// 接收缓冲区
static u8 rxBuffer;
static bit dataReceived = 0;

/**
 * @brief 软件串口初始化
 */
void SoftUart_Init(void)
{
    // 设置引脚初始状态
    UART_TX = 1;  // 空闲状态为高电平
    UART_RX = 1;  // 接收引脚设为输入
    
    // 配置定时器0用于软件串口
    TMOD &= 0xF0;  // 清除定时器0的设置位
    TMOD |= 0x01;  // 设置定时器0为16位定时器模式
    
    TH0 = (TIMER_VALUE >> 8) & 0xFF;  // 定时器高8位
    TL0 = TIMER_VALUE & 0xFF;         // 定时器低8位
    
    ET0 = 1;    // 启用定时器0中断
    EA = 1;     // 启用全局中断
    TR0 = 0;    // 先不启动定时器
    
    dataReceived = 0; // 初始没有收到数据
}

/**
 * @brief 发送一个字节
 * @param dat 要发送的数据
 */
void SoftUart_SendByte(u8 dat)
{
    u8 i;
    bit parity = 0;  // 校验位
    
    // 禁用中断防止冲突
    EA = 0;
    
    // 发送起始位(低电平)
    UART_TX = 0;
    delay_us(104);  // 104us约为1/9600秒
    
    // 发送8位数据，从LSB开始
    for (i = 0; i < 8; i++)
    {
        UART_TX = (dat & 0x01);  // 发送一位数据
        parity ^= (dat & 0x01);  // 更新校验位
        dat >>= 1;               // 移位到下一位
        delay_us(104);           // 每位延时
    }
    
    // 发送停止位(高电平)
    UART_TX = 1;
    delay_us(104);
    
    // 恢复中断
    EA = 1;
}

/**
 * @brief 发送字符串
 * @param str 要发送的字符串
 */
void SoftUart_SendString(u8 *str)
{
    while (*str)
    {
        SoftUart_SendByte(*str++);
    }
}

/**
 * @brief 检查是否有数据可以接收
 * @return 有数据返回1，没有返回0
 */
bit SoftUart_DataReady(void)
{
    return dataReceived;
}

/**
 * @brief 接收一个字节
 * @return 接收到的数据
 */
u8 SoftUart_ReceiveByte(void)
{
    u8 dat = rxBuffer;
    dataReceived = 0;  // 清除数据接收标志
    return dat;
}

/**
 * @brief 接收一个字节(中断方式)
 * 在外部中断0中使用此函数
 */
void SoftUart_ReceiveByteInt(void) interrupt 0
{
    u8 i;
    u8 dat = 0;
    
    // 等待起始位中间
    delay_us(52);
    
    // 检查起始位是否正确
    if (UART_RX == 0)
    {
        // 读取8位数据
        for (i = 0; i < 8; i++)
        {
            delay_us(104);  // 等到下一位中间
            dat >>= 1;      // 右移一位
            if (UART_RX)
                dat |= 0x80;  // 如果是1，设置最高位
        }
        
        // 等待停止位
        delay_us(104);
        
        // 如果停止位正确
        if (UART_RX == 1)
        {
            rxBuffer = dat;
            dataReceived = 1;  // 设置数据接收标志
        }
    }
}