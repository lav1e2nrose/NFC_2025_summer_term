#include "utils.h"
#include <reg52.h>

/**
 * @brief 毫秒延时函数
 * @param ms 延时的毫秒数
 */
void delay_ms(u16 ms)
{
    u16 i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 110; j++);  // 根据11.0592MHz晶振调整
}

/**
 * @brief 微秒延时函数
 * @param us 延时的微秒数
 * 注意：对于微秒级延时，此函数只是近似值
 */
void delay_us(u16 us)
{
    u8 i;
    for (i = 0; i < us; i++)
    {
        _nop_();  // 单周期空指令
    }
}