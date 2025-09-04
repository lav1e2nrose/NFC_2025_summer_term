// AT24C02.c
#include "AT24C02.h"
 
/**
  * @brief  AT24C02 写入一个字节
  * @param  WordAddress 要写入字节的地址
  * @param  Data 要写入的数据
  * @retval 无
  */
void AT24C02_WriteByte(u8 WordAddress, u8 Data)
{
	I2C_Start();
	I2C_SendByte(AT24C02_ADDRESS_READ);
	I2C_ReceiveAck();
	I2C_SendByte(WordAddress);
	I2C_ReceiveAck();
	I2C_SendByte(Data);
	I2C_ReceiveAck();
	I2C_Stop();
}
 
/**
  * @brief  AT24C02读取一个字节
  * @param  WordAddress 要读出字节的地址
  * @retval 读出的数据
  */
u8 AT24C02_ReadByte(u8 WordAddress)
{
	u8 Data;
	I2C_Start();
	I2C_SendByte(AT24C02_ADDRESS_READ);
	I2C_ReceiveAck();
	I2C_SendByte(WordAddress);
	I2C_ReceiveAck();
	I2C_Start();
	// 读地址
	I2C_SendByte(AT24C02_ADDRESS_WRITE);
	I2C_ReceiveAck();
	Data = I2C_ReceiveByte();
	I2C_SendAck(1);
	I2C_Stop();
	return Data;
}

u8 AT24C02_WriteCard(CARD Data, u8 pos) // 按位置写卡，每次写入命令之间需要延时
{
	u8 addr = 0x00 + (ID_LEN + STUID_LEN) * pos, i;
	for (i = 0; i < ID_LEN; i++)
	{
		AT24C02_WriteByte(addr + i, Data.id[i]);
		delay_ms(50);
	}
	for (i = 0; i < STUID_LEN; i++)
	{
		AT24C02_WriteByte(addr + ID_LEN + i, Data.stuid[i]);
		delay_ms(50);
	}
	return 0;
}

// u8* AT24C02_ReadCard(u8* ID)
// {
// 	u8 addr = 0x00, i, id[ID_LEN], stuid[STUID_LEN] = {0};
// 	for (i = 0; i < ID_LEN; i++)
// 	{
// 		id[i] = AT24C02_ReadByte(addr + i);
// 	}
// 	while (strlen(id))
// 	{
// 		if (!strcmp(ID, id))
// 		{
// 			for (i = 0; i < STUID_LEN; i++)
// 			{
// 				stuid[i] = AT24C02_ReadByte(addr + ID_LEN + i);
// 			}
// 			// for (i = 0; i < NAME_LEN; i++)
// 			// {
// 			// 	stu.name[i] = AT24C02_ReadByte(addr + ID_LEN + STUID_LEN + i);
// 			// }
// 		}
// 		else
// 		{
// 			if (addr > 0xFF - (ID_LEN + STUID_LEN))
// 			{
// 				break;
// 			}
// 			addr += ID_LEN + STUID_LEN;
// 			for (i = 0; i < ID_LEN; i++)
// 			{
// 				id[i] = AT24C02_ReadByte(addr + i);
// 			}
// 		}
// 	}
// 	return stuid;
// }

void AT24C02_ReadCard_pos(u8 pos, CARD *card) // 按位置读卡，每次读取之间需要延时
{
	u8 addr = 0x00 + (ID_LEN + STUID_LEN) * pos, i;
	for (i = 0; i < ID_LEN; i++)
	{
		card->id[i] = AT24C02_ReadByte(addr + i);
		delay_ms(50);
	}
	for (i = 0; i < STUID_LEN; i++)
	{
		card->stuid[i] = AT24C02_ReadByte(addr + ID_LEN + i);
		delay_ms(50);
	}
}

void AT24C02_ReadAll(u8 cards[3][11]) // 读取所有卡（只读取卡号以节省空间）
{
	u8 addr = 0x00, i, j;
	for (j = 0; j < 3; j++)
	{
		for (i = 0; i < ID_LEN; i++)
		{
			cards[j][i] = AT24C02_ReadByte(addr + i);
			delay_ms(50);
		}
		addr += ID_LEN + STUID_LEN;
	}
}