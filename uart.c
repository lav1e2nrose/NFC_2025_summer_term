#include "uart.h"

// ������Ϊ9600������11.0592MHz
// ��ʱ������ֵ
#define TIMER_VALUE (65536 - 11059200/12/9600)

// ���ջ�����
static u8 rxBuffer;
static bit dataReceived = 0;

/**
 * @brief ������ڳ�ʼ��
 */
void SoftUart_Init(void)
{
    // �������ų�ʼ״̬
    UART_TX = 1;  // ����״̬Ϊ�ߵ�ƽ
    UART_RX = 1;  // ����������Ϊ����
    
    // ���ö�ʱ��0�����������
    TMOD &= 0xF0;  // �����ʱ��0������λ
    TMOD |= 0x01;  // ���ö�ʱ��0Ϊ16λ��ʱ��ģʽ
    
    TH0 = (TIMER_VALUE >> 8) & 0xFF;  // ��ʱ����8λ
    TL0 = TIMER_VALUE & 0xFF;         // ��ʱ����8λ
    
    ET0 = 1;    // ���ö�ʱ��0�ж�
    EA = 1;     // ����ȫ���ж�
    TR0 = 0;    // �Ȳ�������ʱ��
    
    dataReceived = 0; // ��ʼû���յ�����
}

/**
 * @brief ����һ���ֽ�
 * @param dat Ҫ���͵�����
 */
void SoftUart_SendByte(u8 dat)
{
    u8 i;
    bit parity = 0;  // У��λ
    
    // �����жϷ�ֹ��ͻ
    EA = 0;
    
    // ������ʼλ(�͵�ƽ)
    UART_TX = 0;
    delay_us(104);  // 104usԼΪ1/9600��
    
    // ����8λ���ݣ���LSB��ʼ
    for (i = 0; i < 8; i++)
    {
        UART_TX = (dat & 0x01);  // ����һλ����
        parity ^= (dat & 0x01);  // ����У��λ
        dat >>= 1;               // ��λ����һλ
        delay_us(104);           // ÿλ��ʱ
    }
    
    // ����ֹͣλ(�ߵ�ƽ)
    UART_TX = 1;
    delay_us(104);
    
    // �ָ��ж�
    EA = 1;
}

/**
 * @brief �����ַ���
 * @param str Ҫ���͵��ַ���
 */
void SoftUart_SendString(u8 *str)
{
    while (*str)
    {
        SoftUart_SendByte(*str++);
    }
}

/**
 * @brief ����Ƿ������ݿ��Խ���
 * @return �����ݷ���1��û�з���0
 */
bit SoftUart_DataReady(void)
{
    return dataReceived;
}

/**
 * @brief ����һ���ֽ�
 * @return ���յ�������
 */
u8 SoftUart_ReceiveByte(void)
{
    u8 dat = rxBuffer;
    dataReceived = 0;  // ������ݽ��ձ�־
    return dat;
}

/**
 * @brief ����һ���ֽ�(�жϷ�ʽ)
 * ���ⲿ�ж�0��ʹ�ô˺���
 */
void SoftUart_ReceiveByteInt(void) interrupt 0
{
    u8 i;
    u8 dat = 0;
    
    // �ȴ���ʼλ�м�
    delay_us(52);
    
    // �����ʼλ�Ƿ���ȷ
    if (UART_RX == 0)
    {
        // ��ȡ8λ����
        for (i = 0; i < 8; i++)
        {
            delay_us(104);  // �ȵ���һλ�м�
            dat >>= 1;      // ����һλ
            if (UART_RX)
                dat |= 0x80;  // �����1���������λ
        }
        
        // �ȴ�ֹͣλ
        delay_us(104);
        
        // ���ֹͣλ��ȷ
        if (UART_RX == 1)
        {
            rxBuffer = dat;
            dataReceived = 1;  // �������ݽ��ձ�־
        }
    }
}