#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include <reg52.h>
#include "utils.h"

// ��������ͨ������
sbit BT_TX = P2^0;  // ���ģ��ķ�������
sbit BT_RX = P2^1;  // ���ģ��Ľ�������

// ������ʼ������
void BT_Init(void);

// ����һ���ֽ�
void BT_SendByte(unsigned char dat);

// �����ַ���
void BT_SendString(unsigned char *str);

// ���Ϳ���Ϣ������
void BT_SendCardInfo(void);

#endif