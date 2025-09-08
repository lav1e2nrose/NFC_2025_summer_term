#include <reg52.h>
#include "uart.h"
#include "bluetooth.h"
#include "AT24C02.h"
#include "oled.h"
#include "ui.h"
#include "utils.h"

// ���尴��
sbit KEY1 = P3^0;  // ���Ͱ���
sbit KEY2 = P3^1;  // ���հ���

// ȫ�ֱ���
CARD receivedCard;   // ���յ��Ŀ�Ƭ����

/**
 * @brief ��ⰴ��
 * @return ����ֵ��1��ʾKEY1��2��ʾKEY2��0��ʾ�ް���
 */
u8 KEY_Scan(void)
{
    if (KEY1 == 0)
    {
        delay_ms(10);  // ����
        if (KEY1 == 0)
        {
            while (KEY1 == 0);  // �ȴ������ͷ�
            return 1;
        }
    }
    
    if (KEY2 == 0)
    {
        delay_ms(10);  // ����
        if (KEY2 == 0)
        {
            while (KEY2 == 0);  // �ȴ������ͷ�
            return 2;
        }
    }
    
    return 0;  // �ް�������
}

/**
 * @brief ������
 */
void main(void)
{
    u8 key;
    
    // ��ʼ������ģ��
    UI_Init();          // ��ʼ���û�����
    Bluetooth_Init();   // ��ʼ������
    
    // ������ѭ��
    while (1)
    {
        // ��ⰴ��
        key = KEY_Scan();
        
        if (key == 1)  // ����KEY1�����Ϳ�Ƭ����
        {
            UI_Update(UI_SENDING);
            Bluetooth_SendAllCards();  // �������п�Ƭ����
            UI_Update(UI_IDLE);
        }
        
        // ������������
        Bluetooth_Process();
        
        // ��������״̬����UI
        if (Bluetooth_GetStatus() == BT_SENDING)
        {
            UI_Update(UI_SENDING);
        }
        else if (Bluetooth_GetStatus() == BT_FINISHED)
        {
            // ������ɣ���ʾ���յ�������
            UI_ShowReceived(receivedCard);
        }
        else if (Bluetooth_GetStatus() == BT_IDLE)
        {
            UI_Update(UI_IDLE);
        }
    }
}