#ifndef _BUTTON_H_
#define _BUTTON_H_

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define u8 unsigned char
#define u32 unsigned int

#include <reg52.h>
#include <INTRINS.H>

sbit BTN_SW3 = P3^2;
sbit BTN_LU = P1^5;
sbit BTN_LD = P1^4;
sbit BTN_RU = P1^7;
sbit BTN_RD = P1^6;

void delay_ms(u32 ms);

u8 check_sw3();
u8 check_lu();
u8 check_ld();
u8 check_ru();
u8 check_rd();

#endif