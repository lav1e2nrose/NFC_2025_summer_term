#include "menu.h"

void display_list(u8 *title, u8 list[5][16], u32 length, u32 pos, SELECT select)
{
	u32 i, page_pos = 0;
    OLED_ShowString((120 - strlen(title) * 8) / 2, 0, title);
	page_pos = (pos > 2)? (pos - 2): 0;
    if (!page_pos)
    {
        for (i = 0; i < MIN(length, 3); i++)
        {
			if (select == YES)
			{
				if (i == pos)
				{
					OLED_ShowString(0, 2 * i + 2, "*");
				}
				else
				{
					OLED_ShowString(0, 2 * i + 2, " ");
				}
				OLED_ShowString(10, 2 * i + 2, list[i]);
			}
			else
			{
				OLED_ShowString(0, 2 * i + 2, list[i]);
			}
        }
    }
	else
	{
		for (i = page_pos; i <= pos; i++)
		{
			OLED_ShowString(0, 2, " ");
			OLED_ShowString(0, 4, " ");
			OLED_ShowString(0, 6, "*");
			OLED_ShowString(10, 2 * (i - page_pos) + 2, list[i]);
		}
	}
	if (select == YES && length - page_pos > 3)
	{
		OLED_ShowChar(120, 6, 127);
	}
	else
	{
		OLED_ShowString(120, 6, " ");
	}
}