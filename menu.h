#ifndef _MENU_H_
#define _MENU_H_

#include "utils.h"
#include "oled.h"
#include <string.h>

typedef enum select
{
	YES, NO
} SELECT;

void display_list(u8 *title, u8 list[5][16], u32 length, u32 pos, SELECT select);

#endif