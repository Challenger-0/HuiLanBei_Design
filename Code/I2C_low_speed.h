//手动把SDA和SCL调到双向口或者开漏模式(如果有上拉电阻)！
//不用单独写应答！发送或接收会自动应答！发送的返回值是应答情况，接收的参数也是，用I2C_ACK和I2C_NAK就行
#ifndef _I2C_LOW_SPEED_H_
#define _I2C_LOW_SPEED_H_

#include"STC15W.h"


#include "dataTypeDef.h"

#include "INTRINS.H"
#define I2C_READ 1
#define I2C_WRITE 0

#define I2C_ACK 0
#define I2C_NAK 1

uint8 I2CLReceive(uint8 ack);
uint8 I2CLSend(uint8 dat);
void I2CLEnd();
uint8 I2CLStart(uint8 addr, uint8 rw);
void I2CLInit();
void I2CLSendData(uint8 *dat, uint16 len);
#endif