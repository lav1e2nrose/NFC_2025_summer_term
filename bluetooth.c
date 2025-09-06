#include "bluetooth.h"
#include "AT24C02.h"

// ����ģ���ʼ��
void BT_Init(void)
{
    // ��������ͨ�����ų�ʼ״̬
    BT_TX = 1;  // ����״̬Ϊ�ߵ�ƽ
    
    // ����ģ��һ�����ϵ���Զ�����AT����ģʽ��ͨ��ģʽ
    // �˴�����ͨ������һЩAT��������������ģ��
    delay_ms(500);  // �ȴ�����ģ���ȶ�
}

// ����һ���ֽ����ݣ����ģ��UART���ͣ�
void BT_SendByte(unsigned char dat)
{
    unsigned char i;
    
    // ������ʼλ
    BT_TX = 0;
    delay_ms(1);  // ��ʱԼ9600bps��һλʱ�䣬ʹ��delay_ms����delay_us
    
    // ����8λ����λ
    for(i = 0; i < 8; i++)
    {
        BT_TX = (dat & 0x01) ? 1 : 0;  // �ӵ�λ��ʼ����
        dat >>= 1;
        delay_ms(1);
    }
    
    // ����ֹͣλ
    BT_TX = 1;
    delay_ms(1);
}

// �����ַ���
void BT_SendString(unsigned char *str)
{
    while(*str)
    {
        BT_SendByte(*str++);
    }
}

// ���Ϳ���Ϣ������
void BT_SendCardInfo(void)
{
    unsigned char i;
    CARD card;
    
    BT_SendString("Card Info in EEPROM:\r\n");
    
    // ��EEPROM��ȡ���п�Ƭ��Ϣ��ͨ����������
    for(i = 0; i < 3; i++)
    {
        AT24C02_ReadCard_pos(i, &card);
        
        // ���Ϳ�Ƭ���
        BT_SendString("Card ");
        BT_SendByte('1' + i);
        BT_SendString(": ");
        
        // ���Ϳ�ID
        BT_SendString("ID=");
        BT_SendString(card.id);
        BT_SendString(", ");
        
        // ����ѧ��
        BT_SendString("Student ID=");
        BT_SendString(card.stuid);
        BT_SendString("\r\n");
        
        delay_ms(100);  // ������ʱ���������ݷ���̫��
    }
    
    BT_SendString("Transfer complete.\r\n");
}