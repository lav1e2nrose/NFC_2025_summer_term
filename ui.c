#include "ui.h"
#include "bluetooth.h"

/**
 * @brief 初始化用户界面
 */
void UI_Init(void)
{
    OLED_Init();       // 初始化OLED
    OLED_Clear();      // 清屏
    UI_ShowIdle();     // 显示空闲界面
}

/**
 * @brief 更新用户界面
 * @param status 当前状态
 */
void UI_Update(u8 status)
{
    static u8 last_status = 0xFF;
    
    // 如果状态没变，不更新
    if (status == last_status)
        return;
    
    OLED_Clear();  // 清屏
    
    switch (status)
    {
        case UI_IDLE:
            UI_ShowIdle();
            break;
        case UI_SENDING:
            UI_ShowSending();
            break;
        case UI_RECEIVED:
            // 显示接收到的数据，需要在调用前设置好接收到的卡片数据
            // UI_ShowReceived(receivedCard);
            break;
        default:
            UI_ShowIdle();
            break;
    }
    
    last_status = status;  // 更新上一次状态
}

/**
 * @brief 显示空闲界面
 */
void UI_ShowIdle(void)
{
    OLED_ShowString(0, 0, "BT Card Transfer");
    OLED_ShowString(0, 2, "Status: Idle");
    OLED_ShowString(0, 4, "Press K1:Send");
    OLED_ShowString(0, 6, "Press K2:Recv");
}

/**
 * @brief 显示发送界面
 */
void UI_ShowSending(void)
{
    OLED_ShowString(0, 0, "BT Card Transfer");
    OLED_ShowString(0, 2, "Status: Sending");
    OLED_ShowString(0, 4, "Please wait...");
}

/**
 * @brief 显示接收到的数据
 * @param card 接收到的卡片数据
 */
void UI_ShowReceived(CARD card)
{
    u8 i;
    OLED_ShowString(0, 0, "Card Received");
    
    // 显示卡ID
    OLED_ShowString(0, 2, "ID:");
    for (i = 0; i < ID_LEN && i < 10; i++) {
        OLED_ShowChar(24 + i*8, 2, card.id[i]);
    }
    
    // 显示学生ID
    OLED_ShowString(0, 4, "Student ID:");
    for (i = 0; i < STUID_LEN && i < 10; i++) {
        OLED_ShowChar(0 + i*8, 6, card.stuid[i]);
    }
}