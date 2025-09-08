#include "ui.h"
#include "bluetooth.h"

/**
 * @brief ��ʼ���û�����
 */
void UI_Init(void)
{
    OLED_Init();       // ��ʼ��OLED
    OLED_Clear();      // ����
    UI_ShowIdle();     // ��ʾ���н���
}

/**
 * @brief �����û�����
 * @param status ��ǰ״̬
 */
void UI_Update(u8 status)
{
    static u8 last_status = 0xFF;
    
    // ���״̬û�䣬������
    if (status == last_status)
        return;
    
    OLED_Clear();  // ����
    
    switch (status)
    {
        case UI_IDLE:
            UI_ShowIdle();
            break;
        case UI_SENDING:
            UI_ShowSending();
            break;
        case UI_RECEIVED:
            // ��ʾ���յ������ݣ���Ҫ�ڵ���ǰ���úý��յ��Ŀ�Ƭ����
            // UI_ShowReceived(receivedCard);
            break;
        default:
            UI_ShowIdle();
            break;
    }
    
    last_status = status;  // ������һ��״̬
}

/**
 * @brief ��ʾ���н���
 */
void UI_ShowIdle(void)
{
    OLED_ShowString(0, 0, "BT Card Transfer");
    OLED_ShowString(0, 2, "Status: Idle");
    OLED_ShowString(0, 4, "Press K1:Send");
    OLED_ShowString(0, 6, "Press K2:Recv");
}

/**
 * @brief ��ʾ���ͽ���
 */
void UI_ShowSending(void)
{
    OLED_ShowString(0, 0, "BT Card Transfer");
    OLED_ShowString(0, 2, "Status: Sending");
    OLED_ShowString(0, 4, "Please wait...");
}

/**
 * @brief ��ʾ���յ�������
 * @param card ���յ��Ŀ�Ƭ����
 */
void UI_ShowReceived(CARD card)
{
    u8 i;
    OLED_ShowString(0, 0, "Card Received");
    
    // ��ʾ��ID
    OLED_ShowString(0, 2, "ID:");
    for (i = 0; i < ID_LEN && i < 10; i++) {
        OLED_ShowChar(24 + i*8, 2, card.id[i]);
    }
    
    // ��ʾѧ��ID
    OLED_ShowString(0, 4, "Student ID:");
    for (i = 0; i < STUID_LEN && i < 10; i++) {
        OLED_ShowChar(0 + i*8, 6, card.stuid[i]);
    }
}