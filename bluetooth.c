#include "bluetooth.h"
#include "oled.h"

// ȫ�ֱ���
static u8 bt_status = BT_IDLE;          // ����״̬
static CARD receivedCard;               // ���յ��Ŀ�Ƭ����
static u8 receiveBuffer[20];            // ���ջ�����
static u8 receiveIndex = 0;             // ��������

/**
 * @brief ����ģ���ʼ��
 */
void Bluetooth_Init(void)
{
    SoftUart_Init();     // ��ʼ���������
    bt_status = BT_IDLE; // ���ó�ʼ״̬Ϊ����
}

/**
 * @brief ���Ϳ�Ƭ����
 * @param card Ҫ���͵Ŀ�Ƭ����
 */
void Bluetooth_SendCard(CARD card)
{
    u8 i;
    
    bt_status = BT_SENDING;  // ����״̬Ϊ���ڷ���
    
    // ������ʼ���
    SoftUart_SendString("CARD:");
    
    // ���Ϳ�ID
    for (i = 0; i < ID_LEN; i++) {
        SoftUart_SendByte(card.id[i]);
    }
    
    // ���ͷָ���
    SoftUart_SendString(",");
    
    // ����ѧ��ID
    for (i = 0; i < STUID_LEN; i++) {
        SoftUart_SendByte(card.stuid[i]);
    }
    
    // ���ͽ������
    SoftUart_SendString("#");
    
    bt_status = BT_FINISHED;  // ����״̬Ϊ�������
    delay_ms(100);            // ��ʱһ��ʱ��
    bt_status = BT_IDLE;      // �ָ�����״̬
}

/**
 * @brief �������п�Ƭ����
 */
void Bluetooth_SendAllCards(void)
{
    CARD card;
    u8 i;
    
    for (i = 0; i < 3; i++) {  // ������3�ſ�
        AT24C02_ReadCard_pos(i, &card);  // ��ȡ��Ƭ����
        Bluetooth_SendCard(card);        // ���Ϳ�Ƭ����
        delay_ms(500);                   // ���ſ�֮���ӳ�
    }
}

/**
 * @brief ��ȡ��ǰ����״̬
 * @return ����״̬
 */
u8 Bluetooth_GetStatus(void)
{
    return bt_status;
}

/**
 * @brief ������յ�������
 * �������յ������ݲ�����
 */
void Bluetooth_ProcessReceivedData(void)
{
    // �򵥵Ľ�����CARD:ID����,STUID����#
    u8 i = 5;  // ����"CARD:"
    u8 j = 0;
    
    // ��ȡID
    while (i < receiveIndex && receiveBuffer[i] != ',') {
        if (j < ID_LEN) {
            receivedCard.id[j++] = receiveBuffer[i++];
        } else {
            i++;  // ��������Խ��
        }
    }
    
    // ���ʣ��ID�ռ�
    while (j < ID_LEN) {
        receivedCard.id[j++] = 0;
    }
    
    // ��������
    i++;
    
    // ��ȡSTUID
    j = 0;
    while (i < receiveIndex && receiveBuffer[i] != '#') {
        if (j < STUID_LEN) {
            receivedCard.stuid[j++] = receiveBuffer[i++];
        } else {
            i++;  // ��������Խ��
        }
    }
    
    // ���ʣ��STUID�ռ�
    while (j < STUID_LEN) {
        receivedCard.stuid[j++] = 0;
    }
    
    // ���浽EEPROM (��ѡ)
    // AT24C02_WriteCard(receivedCard, 0);  // �洢����һ��λ��
    
    // ���ý��ջ�����
    receiveIndex = 0;
}

/**
 * @brief ��������
 * ����ѭ���е��ã�����Ƿ������ݽ���
 */
void Bluetooth_Process(void)
{
    // ����Ƿ������ݽ���
    if (SoftUart_DataReady())
    {
        u8 dat = SoftUart_ReceiveByte();
        
        // ���֮ǰ�ǿ���״̬���ҽ��յ�'C'����ʼ����
        if (bt_status == BT_IDLE && dat == 'C' && receiveIndex == 0)
        {
            bt_status = BT_RECEIVING;
            receiveBuffer[receiveIndex++] = dat;
        }
        // ������ڽ�������
        else if (bt_status == BT_RECEIVING)
        {
            // ������ջ�����
            if (receiveIndex < 20)
            {
                receiveBuffer[receiveIndex++] = dat;
            }
            
            // ����Ƿ�������
            if (dat == '#')
            {
                Bluetooth_ProcessReceivedData();
                bt_status = BT_FINISHED;
                delay_ms(100);
                bt_status = BT_IDLE;
            }
        }
    }
}