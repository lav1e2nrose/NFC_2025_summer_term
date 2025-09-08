#ifndef __UI_H__
#define __UI_H__

#include "oled.h"

// ����״̬
#define UI_IDLE      0  // ���н���
#define UI_SENDING   1  // ���ͽ���
#define UI_RECEIVED  2  // ���ս���

// ��������
void UI_Init(void);                     // ��ʼ������
void UI_Update(u8 status);              // ���½���״̬
void UI_ShowIdle(void);                 // ��ʾ���н���
void UI_ShowSending(void);              // ��ʾ���ͽ���
void UI_ShowReceived(CARD card);        // ��ʾ���յ�������

#endif