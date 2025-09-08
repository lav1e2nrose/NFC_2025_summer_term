#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "uart.h"
#include "AT24C02.h"

// ����״̬����
#define BT_IDLE     0   // ����״̬
#define BT_SENDING  1   // ���ڷ���
#define BT_RECEIVING 2  // ���ڽ���
#define BT_FINISHED 3   // �������

// ��������
void Bluetooth_Init(void);                      // ������ʼ��
void Bluetooth_SendCard(CARD card);             // ���Ϳ�Ƭ����
void Bluetooth_SendAllCards(void);              // �������п�Ƭ����
u8 Bluetooth_GetStatus(void);                   // ��ȡ��ǰ����״̬
void Bluetooth_Process(void);                   // ����������������ѭ���е���
void Bluetooth_ReceiveData(void);               // ��������

#endif