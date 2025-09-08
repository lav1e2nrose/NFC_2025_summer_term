#include "bluetooth.h"
#include "oled.h"

// 全局变量
static u8 bt_status = BT_IDLE;          // 蓝牙状态
static CARD receivedCard;               // 接收到的卡片数据
static u8 receiveBuffer[20];            // 接收缓冲区
static u8 receiveIndex = 0;             // 接收索引

/**
 * @brief 蓝牙模块初始化
 */
void Bluetooth_Init(void)
{
    SoftUart_Init();     // 初始化软件串口
    bt_status = BT_IDLE; // 设置初始状态为空闲
}

/**
 * @brief 发送卡片数据
 * @param card 要发送的卡片数据
 */
void Bluetooth_SendCard(CARD card)
{
    u8 i;
    
    bt_status = BT_SENDING;  // 设置状态为正在发送
    
    // 发送起始标记
    SoftUart_SendString("CARD:");
    
    // 发送卡ID
    for (i = 0; i < ID_LEN; i++) {
        SoftUart_SendByte(card.id[i]);
    }
    
    // 发送分隔符
    SoftUart_SendString(",");
    
    // 发送学生ID
    for (i = 0; i < STUID_LEN; i++) {
        SoftUart_SendByte(card.stuid[i]);
    }
    
    // 发送结束标记
    SoftUart_SendString("#");
    
    bt_status = BT_FINISHED;  // 设置状态为传输完成
    delay_ms(100);            // 延时一段时间
    bt_status = BT_IDLE;      // 恢复空闲状态
}

/**
 * @brief 发送所有卡片数据
 */
void Bluetooth_SendAllCards(void)
{
    CARD card;
    u8 i;
    
    for (i = 0; i < 3; i++) {  // 假设有3张卡
        AT24C02_ReadCard_pos(i, &card);  // 读取卡片数据
        Bluetooth_SendCard(card);        // 发送卡片数据
        delay_ms(500);                   // 两张卡之间延迟
    }
}

/**
 * @brief 获取当前蓝牙状态
 * @return 蓝牙状态
 */
u8 Bluetooth_GetStatus(void)
{
    return bt_status;
}

/**
 * @brief 处理接收到的数据
 * 解析接收到的数据并保存
 */
void Bluetooth_ProcessReceivedData(void)
{
    // 简单的解析：CARD:ID数据,STUID数据#
    u8 i = 5;  // 跳过"CARD:"
    u8 j = 0;
    
    // 提取ID
    while (i < receiveIndex && receiveBuffer[i] != ',') {
        if (j < ID_LEN) {
            receivedCard.id[j++] = receiveBuffer[i++];
        } else {
            i++;  // 避免数组越界
        }
    }
    
    // 填充剩余ID空间
    while (j < ID_LEN) {
        receivedCard.id[j++] = 0;
    }
    
    // 跳过逗号
    i++;
    
    // 提取STUID
    j = 0;
    while (i < receiveIndex && receiveBuffer[i] != '#') {
        if (j < STUID_LEN) {
            receivedCard.stuid[j++] = receiveBuffer[i++];
        } else {
            i++;  // 避免数组越界
        }
    }
    
    // 填充剩余STUID空间
    while (j < STUID_LEN) {
        receivedCard.stuid[j++] = 0;
    }
    
    // 保存到EEPROM (可选)
    // AT24C02_WriteCard(receivedCard, 0);  // 存储到第一个位置
    
    // 重置接收缓冲区
    receiveIndex = 0;
}

/**
 * @brief 接收数据
 * 在主循环中调用，检查是否有数据接收
 */
void Bluetooth_Process(void)
{
    // 检查是否有数据接收
    if (SoftUart_DataReady())
    {
        u8 dat = SoftUart_ReceiveByte();
        
        // 如果之前是空闲状态，且接收到'C'，开始接收
        if (bt_status == BT_IDLE && dat == 'C' && receiveIndex == 0)
        {
            bt_status = BT_RECEIVING;
            receiveBuffer[receiveIndex++] = dat;
        }
        // 如果正在接收数据
        else if (bt_status == BT_RECEIVING)
        {
            // 存入接收缓冲区
            if (receiveIndex < 20)
            {
                receiveBuffer[receiveIndex++] = dat;
            }
            
            // 检查是否接收完成
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