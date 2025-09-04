#include "utils.h"

void delay_ms(u32 ms)
{
	u32 i, j;
	for (i = ms; i > 0; i--)
		for (j = 114; j > 0; j--);
}

u8 check_sw3()
{
	if (!BTN_SW3)
	{
		delay_ms(1);
		return (BTN_SW3)? 0: 1;
	}
	return 0;
}

u8 check_lu()
{
	if (!BTN_LU)
	{
		delay_ms(1);
		return (BTN_LU)? 0: 1;
	}
	return 0;
}

u8 check_ld()
{
	if (!BTN_LD)
	{
		delay_ms(1);
		return (BTN_LD)? 0: 1;
	}
	return 0;
}

u8 check_ru()
{
	if (!BTN_RU)
	{
		delay_ms(1);
		return (BTN_RU)? 0: 1;
	}
	return 0;
}

u8 check_rd()
{
	if (!BTN_RD)
	{
		delay_ms(1);
		return (BTN_RD)? 0: 1;
	}
	return 0;
}