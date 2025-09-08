#include "utils.h"
#include <reg52.h>

/**
 * @brief ������ʱ����
 * @param ms ��ʱ�ĺ�����
 */
void delay_ms(u16 ms)
{
    u16 i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 110; j++);  // ����11.0592MHz�������
}

/**
 * @brief ΢����ʱ����
 * @param us ��ʱ��΢����
 * ע�⣺����΢�뼶��ʱ���˺���ֻ�ǽ���ֵ
 */
void delay_us(u16 us)
{
    u8 i;
    for (i = 0; i < us; i++)
    {
        _nop_();  // �����ڿ�ָ��
    }
}