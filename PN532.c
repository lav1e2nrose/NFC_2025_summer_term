#include "PN532.h"
#include <INTRINS.H>
#include <string.h>

u8 rec_str[40];	// 串口接收的数据
u32 rec_cnt;		// 串口接收个数
u8 prt_str[16];

sbit tx = P3^1;

// 发送单个字节信号
void send_Bytee(u8 ch)
{
	SBUF = ch;
	while (!TI);			 // 等待发送数据完成
	TI = 0;					 // 清除发送完成标志位 
}

void send_Byte(u8 ch)		// 使用软件模拟 115200 波特率，_nop_() 个数手动调整
{
	tx = 1;
	_nop_(); _nop_(); _nop_(); _nop_(); _nop_();
    tx = 0;
	_nop_(); _nop_(); _nop_(); _nop_();
	tx = ch & 0x01;
    ch = ch >> 1;
	_nop_();
	tx = ch & 0x01;
  	ch = ch >> 1;
	_nop_();
	tx = ch & 0x01;
  	ch = ch >> 1;
	_nop_();
	tx = ch & 0x01;
  	ch = ch >> 1;
	_nop_();
	tx = ch & 0x01;
  	ch = ch >> 1;
	_nop_();
	tx = ch & 0x01;
  	ch = ch >> 1;
	_nop_();
	tx = ch & 0x01;
  	ch = ch >> 1;
	_nop_();
	tx = ch & 0x01;
  	ch = ch >> 1;
	_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	tx = 1;
}

void send_Command(u8* command, u8 length) // 使用软件模拟 115200 波特率发送数据
{
    u8 i;
    for (i = 0; i < length; i++)
    {
        send_Byte(command[i]);
    }
}

void send_Commandd(u8* command, u8 length) // 使用硬件串口 9600 波特率发送数据
{
    u8 i;
    for (i = 0; i < length; i++)
    {
        send_Bytee(command[i]);
    }
}

void PN532_Init()
{
    u8 command[24] = {0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x03, 0xFD, 0xD4, 0x14, 0x01, 0x17, 0x00};
	u8 baud[10] = {0x00, 0x00, 0xFF, 0x03, 0xFD, 0xD4, 0x10, 0x00, 0x1C, 0x00};
	send_Command(command, 24); // 激活
	delay_ms(100);
    send_Command(baud, 10); // 调整波特率，需要发两遍
	delay_ms(1000);
	send_Command(baud, 10);
}

void PN532_findUID()
{
    u8 command[11] = {0x00, 0x00, 0xFF, 0x04, 0xFC, 0xD4, 0x4A, 0x02, 0x00, 0xE0, 0x00};
	memset(rec_str, 0, sizeof(rec_str));
	send_Commandd(command, 11); // 寻卡
	rec_cnt = 0;
}

void trans_uid() // 将读到的 16 进制 ID 转换为字符串
{	
	u8 i;
	for(i = 0; i < 4; i++)
	{
		prt_str[2 * i] = rec_str[i + 2] / 16;
		prt_str[2 * i + 1] = rec_str[i + 2] % 16;
	}
	for(i = 0; i < 8; i++)
	{
		(prt_str[i] > 9)? (prt_str[i] += 0x37): (prt_str[i] += 0x30);
	}
} 
