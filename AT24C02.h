#ifndef _AT24C02_H_
#define _AT24C02_H_

#include "I2C.h"
#include "utils.h"
#include <string.h>

// SLAVE ADDRESS + W 为 0xA0，SLAVE ADDRESS + R 为 0xA1
#define AT24C02_ADDRESS_READ		0xA0
#define AT24C02_ADDRESS_WRITE		0xA1
#define ID_LEN 9
#define STUID_LEN 11

typedef struct card
{
    u8 id[ID_LEN];
    u8 stuid[STUID_LEN];
} CARD;

void AT24C02_WriteByte(u8 WordAddress, u8 Data);

u8 AT24C02_ReadByte(u8 WordAddress);

u8 AT24C02_WriteCard(CARD Data, u8 pos);

u8* AT24C02_ReadCard(u8* ID);

void AT24C02_ReadCard_pos(u8 pos, CARD *card);

void AT24C02_ReadAll(u8 cards[3][11]);

#endif