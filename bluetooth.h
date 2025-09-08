#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include <reg52.h>
#include "utils.h"
#include "AT24C02.h"

/* ����ӳ�䣺������ֿ�һ��
   P2.0 -> TX (����� HC-05 RXD)
   P2.1 <- RX (������ HC-05 TXD) */
sbit BT_TX = P2^0;
sbit BT_RX = P2^1;

/* ������/Э�鳣�� */
#define BT_BAUD_RATE      9600
#define BT_HDR0           0x55
#define BT_HDR1           0xAA
#define BT_TYPE_CARD      0x01
#define BT_PAYLOAD_LEN    20    /* 9(ID)+11(STUID) */
#define BT_FRAME_LEN      (2+1+1+BT_PAYLOAD_LEN+1)

/* ��ʼ������ TX ���иߣ� */
void BT_Init(void);

/* ���� 1 �ֽڣ���ȷ 9600bps bit-bang�� */
void BT_SendByte(unsigned char dat);

/* ������ 0 �������ַ��������ֽڷ��ͣ� */
void BT_SendString(unsigned char *str);

/* ���ͻ����� */
void BT_SendBuffer(const unsigned char* buf, unsigned int len);

/* ����ʱ���� 1 �ֽڣ����� 1=�ɹ�, 0=��ʱ/ʧ�ܣ�timeout_ms ���룩 */
bit BT_RecvByteTimeout(unsigned char* out, unsigned int timeout_ms);

/* ֡������/����һ�ſ� */
bit BT_SendCardFrame(const CARD* card);
bit BT_RecvCardFrame(CARD* out, unsigned int timeout_ms);

/* ���ݽӿڣ����� EEPROM �� 3 �ſ�����Ϣ��ʮ�����ƴ�ӡ�� */
void BT_SendCardInfo(void);

#endif