#ifndef _PN532_H_
#define _PN532_H_

#include <reg52.h>
#include "utils.h"

void PN532_Init();
void send_Byte(u8 ch);
void PN532_findUID();
void send_Command(u8* command, u8 length);
void trans_uid();

#endif