#ifndef __UART_H__
#define __UART_H__

#include <reg52.h>
#include "utils.h"

// ����������Ŷ���
sbit UART_TX = P2^1;  // ��������
sbit UART_RX = P2^0;  // ��������

// ��������
void SoftUart_Init(void);       // ������ڳ�ʼ��
void SoftUart_SendByte(u8 dat); // ����һ���ֽ�
void SoftUart_SendString(u8 *str); // �����ַ���
u8 SoftUart_ReceiveByte(void);  // ����һ���ֽ�
bit SoftUart_DataReady(void);   // ����Ƿ������ݿ��Խ���

#endif