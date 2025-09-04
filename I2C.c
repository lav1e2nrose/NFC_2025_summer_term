#include "I2C.h"
 
// 单片机比较慢无需 delay，这个就是按照时序图来的，按图来就行
/**
  * @brief  I2C 开始
  * @param  无
  * @retval 无
  */
void I2C_Start(void)
{
	// 可以理解为初始化，确保一定为高电平
	I2C_SDA = 1;
	I2C_SCL = 1;
	// 按照时序图可得，先 SDA 为 0，再 SCL 为 0
	I2C_SDA = 0;
	I2C_SCL = 0;
}
 
/**
  * @brief  I2C 停止
  * @param  无
  * @retval 无
  */
void I2C_Stop(void)
{
	I2C_SDA = 0;
	I2C_SCL = 1;
	I2C_SDA = 1;
}
 
/**
  * @brief  I2C 发送一个字节
  * @param  Byte 要发送的字节
  * @retval 无
  */
void I2C_SendByte(unsigned char Byte)
{
	unsigned char i;
	for(i = 0; i < 8; i++){
		I2C_SDA = Byte & (0x80 >> i);
		I2C_SCL = 1;
		I2C_SCL = 0;
	}
}
 
/**
  * @brief  I2C 接收一个字节
  * @param  无
  * @retval 接收到的一个字节数据
  */
unsigned char I2C_ReceiveByte(void)
{
	unsigned char i, byte = 0x00;
	// 释放 SDA
	I2C_SDA = 1;
	for(i = 0; i < 8; i++)
	{
		I2C_SCL = 1;
		byte = (I2C_SDA)? (byte | (0x80 >> i)): byte;
		I2C_SCL = 0;
	}
	return byte;
}
 
/**
  * @brief  I2C 发送应答
  * @param  AckBit 应答位，0 为应答，1 为非应答
  * @retval 无
  */
void I2C_SendAck(unsigned char AckBit)
{
	I2C_SDA = AckBit;
	I2C_SCL = 1;
	I2C_SCL = 0;
}
 
/**
  * @brief  I2C 接收应答位
  * @param  无
  * @retval 接收到的应答位，0 为应答，1 为非应答
  */
unsigned char I2C_ReceiveAck(void)
{
	unsigned char AckBit;
	// 释放 SDA
	I2C_SDA = 1;
	I2C_SCL = 1;
	AckBit = I2C_SDA;
	I2C_SCL = 0;
	return AckBit;
}