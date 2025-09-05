#include "oled.h"
#include "utils.h"
#include "PN532.h"
#include "AT24C02.h"
#include "menu.h"

typedef enum mode // 模式来标识各个页面
{
    MENU, PN532, BT, EEPROM, PN532READ, PN532WRITE, PN532CARD, PN532P2P, BTTD, DATA
} MODE;

sbit BUZZER = P2^7; // 蜂鸣器

MODE mode = MENU, prev_mode;

extern u8 rec_str[], prt_str[]; // 用于串口数据接收
extern u32 rec_cnt; // 串口数据量计数

// 已知卡UID和对应学号
code u8 KNOWN_UID1[] = "4EDA9C04";
code u8 KNOWN_UID2[] = "87654321";
code u8 KNOWN_STUID1[] = "3230105625";
code u8 KNOWN_STUID2[] = "3230105626";
code u8 UNKNOWN_STUID[] = "0000000000";

void beep() // 蜂鸣 0.5 秒
{
	BUZZER = 0;
	delay_ms(500);
	BUZZER = 1;
}

void UartInit(void)		// 9600bps @ 11.0592MHz
{
	PCON |= 0x80;		// 使能波特率倍速位 SMOD
	SCON = 0x50;		// 8 位数据，可变波特率
	TMOD &= 0x0F;		// 清除定时器 1 模式位
	TMOD |= 0x20;		// 设定定时器 1 为 8 位自动重装方式
	TL1 = 0xFA;		// 设定定时初值
	TH1 = 0xFA;		// 设定定时器重装值
	ET1 = 0;		// 禁止定时器 1 中断
	TR1 = 1;		// 启动定时器 1
	EA = 1;
	ES = 1;
}

// 比较两个字符串是否相等
bit compareUID(u8 *uid1, u8 *uid2)
{
    u8 i;
    for(i = 0; i < 8; i++) {
        if(uid1[i] != uid2[i])
            return 0;
    }
    return 1;
}

void Usart() interrupt 4 // 串口中断
{
	EA = 0;
	rec_str[rec_cnt++] = SBUF;
	RI = 0; // 清除接收中断标志位
	EA = 1;
}

void display() // 负责调整显示内容与业务逻辑
{
    u8 *title, idata list[4][16], idata buffer[14] = {0}, i, idata cards[3][11], length, pos = 0, id[9], stuid[11], data_pos;
	MODE mode_list[5];
	SELECT select;
	CARD idata card;
	switch (mode)
	{
		case PN532READ: case PN532WRITE: case PN532CARD: case PN532P2P: case BTTD: case DATA:
		{
			if (check_lu())
			{
				prev_mode = mode;
				mode = MENU;
				OLED_Clear();
			}
			break;
		}
	}
    switch(mode)
    {
        case MENU: // 显示菜单
        {
            title = "MENU";
			strcpy(list[0], "PN532         ");
			strcpy(list[1], "BlueTooth     ");
			strcpy(list[2], "EEPROM        ");
			length = 3;
			select = YES;
			break;
        }
		case PN532: // PN532 选项
        {
            title = "PN532";
			strcpy(list[0], "Read          ");
			strcpy(list[1], "Write         ");
			strcpy(list[2], "Card          ");
			strcpy(list[3], "P2P           ");
			length = 4;
			select = YES;
			break;
        }
		case BT: // 蓝牙选项
        {
            title = "BlueTooth";
			strcpy(list[0], "Transfer Data ");
			length = 1;
			select = YES;
			break;
        }
		case PN532READ: // 读卡界面
        {
            title = "PN532 - Read";
			length = 3;
			select = NO;

			rec_cnt = 0;
			PN532_findUID(); // 先渲染读卡中界面，同时发送寻卡指令
			strcpy(list[0], "Reading...");
			strcpy(list[1], "MENU         UP");
			strcpy(list[2], "BACK       DOWN");
			display_list(title, list, length, pos, select);

			while (1) // 进入死循环等待读到卡
			{
				if (rec_cnt > 15)
				{
					rec_cnt = 0;
					beep(); // 蜂鸣提示
					trans_uid();
					strncpy(buffer, prt_str, 8); // 准备显示卡号
					strcpy(card.id, buffer);
					strcpy(list[0], strcat(buffer, "  "));
					strcpy(list[1], "SAVE            ");
					strcpy(list[2], "BACK            ");
					display_list(title, list, length, pos, select);
					while (1) // 进入死循环等待保存 / 不保存
					{
						if (check_lu()) // 按下保存按钮的逻辑
						{
							// 根据UID匹配学号
							if(compareUID(card.id, KNOWN_UID1)) {
								strcpy(card.stuid, KNOWN_STUID1);
							} else if(compareUID(card.id, KNOWN_UID2)) {
								strcpy(card.stuid, KNOWN_STUID2);
							} else {
								strcpy(card.stuid, UNKNOWN_STUID); // 未知卡设为0000000000
							}
							data_pos = AT24C02_ReadByte(0xFF); // 从 EEPROM 处读取保存卡的位置（设定为 3 个位置）
							delay_ms(50);
							AT24C02_WriteCard(card, data_pos); // 写卡进 EEPROM，包括 ID 和学号
							data_pos = (data_pos == 2)? 0: (data_pos + 1);
							delay_ms(50);
							AT24C02_WriteByte(0xFF, data_pos); // 写保存位置入 EEPROM
							mode = MENU; // 重新渲染菜单
							title = "MENU";
							strcpy(list[0], "PN532         ");
							strcpy(list[1], "BlueTooth     ");
							strcpy(list[2], "EEPROM        ");
							select = YES;
							OLED_Clear();
							break;
						}
						if (check_ld())
						{
							mode = MENU;
							title = "MENU";
							strcpy(list[0], "PN532         ");
							strcpy(list[1], "BlueTooth     ");
							strcpy(list[2], "EEPROM        ");
							select = YES;
							OLED_Clear();
							break;
						}
					}
					break;
				}
			}
			break;
        }
		case PN532WRITE:
        {
            title = "PN532 - Write";
			strcpy(list[0], "Writing...");
			strcpy(list[1], "MENU         UP");
			strcpy(list[2], "BACK       DOWN");
			length = 3;
			select = NO;
			break;
        }
		case PN532CARD:
        {
            title = "PN532 - Card";
			strcpy(list[0], "Simulating...");
			strcpy(list[1], "MENU         UP");
			strcpy(list[2], "BACK       DOWN");
			length = 3;
			select = NO;
			break;
        }
		case PN532P2P:
        {
            title = "PN532 - P2P";
			strcpy(list[0], "Connecting...");
			strcpy(list[1], "MENU         UP");
			strcpy(list[2], "BACK       DOWN");
			length = 3;
			select = NO;
			break;
        }
		case BTTD:
        {
            title = "BlueTooth - TD";
			strcpy(list[0], "Transferring...");
			strcpy(list[1], "MENU         UP");
			strcpy(list[2], "BACK       DOWN");
			length = 3;
			select = NO;
			break;
        }
		case EEPROM:
		{
			title = "EEPROM";
			AT24C02_ReadAll(cards); // 从 EEPROM 读取所有卡号
			for (i = 0; i < 3; i++)
			{
				strcpy(list[i], cards[i]);
			}
			length = i;
			select = YES;
			break;
		}
		case DATA:
		{
			strcpy(title, id);
			strcpy(list[0], stuid);
			strcpy(list[1], "MENU         UP");
			strcpy(list[2], "BACK       DOWN");
			length = 3;
			select = NO;
			break;
		}
    }
	switch (mode)
	{
		case MENU: case PN532: case BT: case EEPROM: // 可选菜单的渲染逻辑
		{
			while (1)
			{
				// 通过按键调整选中栏
				pos = (check_ru() && pos > 0)? (pos - 1): pos;
				pos = (check_rd() && pos < length - 1)? (pos + 1): pos;
				display_list(title, list, length, pos, select);
				if (check_ld())
				{
					mode = prev_mode;
					OLED_Clear();
					break;
				}
				if (check_lu()) // 选中的逻辑
				{
					switch (mode)
					{
						case MENU:
						{
							mode_list[0] = PN532;
							mode_list[1] = BT;
							mode_list[2] = EEPROM;
							mode = mode_list[pos];
							OLED_Clear();
							break;
						}
						case PN532:
						{
							mode_list[0] = PN532READ;
							mode_list[1] = PN532WRITE;
							mode_list[2] = PN532CARD;
							mode_list[3] = PN532P2P;
							mode = mode_list[pos];
							OLED_Clear();
							break;
						}
						case BT:
						{
							mode_list[0] = BTTD;
							mode = mode_list[pos];
							OLED_Clear();
							break;
						}
						case EEPROM: // 读出卡的数据
						{
							strcpy(id, cards[pos]);
							AT24C02_ReadCard_pos(pos, &card);
							strcpy(stuid, card.stuid);
							mode = DATA;
							OLED_Clear();
							break;
						}
					}
					break;
				}
			}
			break;
		}
		default:
		{
			display_list(title, list, length, pos, select);
		}
	}
}

void main()
{
	CARD Data = {0};
    OLED_Init();			// 初始化 OLED
    OLED_Clear();
	PN532_Init();
	delay_ms(100);
	UartInit();
	memset(rec_str, 0, sizeof(rec_str));
	rec_cnt = 0;
	while (1)
	{
		BUZZER = (check_sw3())? ~BUZZER: BUZZER;
		display();
	}
}