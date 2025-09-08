#ifndef _I2C_H_
#define _I2C_H_

#include <reg52.h>

// 在引脚部分介绍过了，SCL 是 P21，SDA 是 P20；
sbit I2C_SCL = P3^4;
sbit I2C_SDA = P3^5;

void I2C_Start(void);

void I2C_Stop(void);

void I2C_SendByte(unsigned char Byte);

unsigned char I2C_ReceiveByte(void);

void I2C_SendAck(unsigned char AckBit);

unsigned char I2C_ReceiveAck(void);

#endif