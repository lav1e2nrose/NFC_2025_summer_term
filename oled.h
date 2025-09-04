#ifndef __OLED_H
#define __OLED_H
#include "utils.h"
#define OLED_CMD  0	// 写命令
#define OLED_DATA 1	// 写数据
#define OLED_MODE 0

sbit OLED_RST = P1^2; // 复位
sbit OLED_DC = P1^3; // 数据/命令控制
sbit OLED_SCL = P1^0; // 时钟 D0（SCLK?
sbit OLED_SDIN = P1^1; // D1（MOSI） 数据

#define OLED_CS_Clr()  OLED_CS = 0
#define OLED_CS_Set()  OLED_CS = 1

#define OLED_RST_Clr() OLED_RST = 0
#define OLED_RST_Set() OLED_RST = 1

#define OLED_DC_Clr() OLED_DC = 0
#define OLED_DC_Set() OLED_DC = 1

#define OLED_SCLK_Clr() OLED_SCL = 0
#define OLED_SCLK_Set() OLED_SCL = 1

#define OLED_SDIN_Clr() OLED_SDIN = 0
#define OLED_SDIN_Set() OLED_SDIN = 1;

// OLED 模式设置
// 0: 4 线串行模式
// 1: 并行 8080 模式

#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED端口定义----------------

// OLED 控制用函数
void OLED_WR_Byte(u8 dat, u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x, u8 y, u8 t);
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot);
void OLED_ShowChar(u8 x, u8 y, u8 chr);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2);
void OLED_ShowString(u8 x, u8 y, u8 *p);
void OLED_Set_Pos(u8 x, u8 y);

#endif  
	 



