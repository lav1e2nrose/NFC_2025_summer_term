#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include <reg52.h>
#include "utils.h"
#include "AT24C02.h"

/* 引脚映射：保持与仓库一致
   P2.0 -> TX (输出到 HC-05 RXD)
   P2.1 <- RX (输入自 HC-05 TXD) */
sbit BT_TX = P2^0;
sbit BT_RX = P2^1;

/* 波特率/协议常量 */
#define BT_BAUD_RATE      9600
#define BT_HDR0           0x55
#define BT_HDR1           0xAA
#define BT_TYPE_CARD      0x01
#define BT_PAYLOAD_LEN    20    /* 9(ID)+11(STUID) */
#define BT_FRAME_LEN      (2+1+1+BT_PAYLOAD_LEN+1)

/* 初始化（置 TX 空闲高） */
void BT_Init(void);

/* 发送 1 字节（精确 9600bps bit-bang） */
void BT_SendByte(unsigned char dat);

/* 发送以 0 结束的字符串（逐字节发送） */
void BT_SendString(unsigned char *str);

/* 发送缓冲区 */
void BT_SendBuffer(const unsigned char* buf, unsigned int len);

/* 带超时接收 1 字节；返回 1=成功, 0=超时/失败（timeout_ms 毫秒） */
bit BT_RecvByteTimeout(unsigned char* out, unsigned int timeout_ms);

/* 帧化发送/接收一张卡 */
bit BT_SendCardFrame(const CARD* card);
bit BT_RecvCardFrame(CARD* out, unsigned int timeout_ms);

/* 兼容接口：发送 EEPROM 中 3 张卡的信息（十六进制打印） */
void BT_SendCardInfo(void);

#endif