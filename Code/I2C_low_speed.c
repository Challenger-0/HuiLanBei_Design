#include "I2C_low_speed.h"


#define I2C_SDA P26
#define I2C_SCL P25


void I2CLDelay()		//@22.1184MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	_nop_();
	i = 30;
	while (--i);
}


uint8 I2CLReceive(uint8 ack){
    uint8 dat,EAStatus=EA;

    //EA = 0;//禁止一切中断

    I2C_SDA = 1;//release SDA for receive

    I2C_SCL = 1;
    I2CLDelay();
    dat |= I2C_SDA;
    I2C_SCL = 0;
    dat <<= 1;
    I2CLDelay();

    I2C_SCL = 1;
    I2CLDelay();
    dat |= I2C_SDA;
    I2C_SCL = 0;
    dat <<= 1;
    I2CLDelay();

    I2C_SCL = 1;
    I2CLDelay();
    dat |= I2C_SDA;
    I2C_SCL = 0;
    dat<<=1;
    I2CLDelay();

    I2C_SCL = 1;
    I2CLDelay();
    dat |= I2C_SDA;
    I2C_SCL = 0;
    dat<<=1;
    I2CLDelay();

    I2C_SCL = 1;
    I2CLDelay();
    dat |= I2C_SDA;
    I2C_SCL = 0;
    dat<<=1;
    I2CLDelay();

    I2C_SCL = 1;
    I2CLDelay();
    dat |= I2C_SDA;
    I2C_SCL = 0;
    dat<<=1;
    I2CLDelay();

    I2C_SCL = 1;
    I2CLDelay();
    dat |= I2C_SDA;
    I2C_SCL = 0;
    dat<<=1;
    I2CLDelay();

    I2C_SCL = 1;
    I2CLDelay();
    dat |= I2C_SDA;
    I2C_SCL = 0;
    //dat<<=1;
    I2CLDelay();

    I2C_SDA = ack;
    I2C_SCL = 1;
    I2CLDelay();
    _nop_();
    I2C_SCL = 0;

    //EA = EAStatus;
    return dat;
}

uint8 I2CLSend(uint8 dat){
    uint8 ack, count,EAStatus=EA;

    //EA = 0;//禁用一切中断避免干扰

    I2C_SDA = dat >> 7;//didn't use loop to rise I2C speed
    I2C_SCL = 1;
    I2CLDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CLDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CLDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CLDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CLDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CLDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CLDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CLDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CLDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CLDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CLDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CLDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CLDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CLDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CLDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CLDelay();

    I2C_SDA = 1; //release SDA and wait for ack
    I2C_SCL = 1;
    for (count = 255; I2C_SDA && count;count--)
        ;
    ack = I2C_SDA;
    I2C_SCL = 0;

    //EA = EAStatus;//恢复中断状态；

    return ack;
}

void I2CLEnd(){
    I2C_SDA = 0;
    I2CLDelay();
    I2C_SCL = 1;
    I2CLDelay();
    I2CLDelay();
    I2C_SDA = 1;
}

uint8 I2CLStart(uint8 addr,uint8 rw){
    I2C_SDA = 1;
    I2CLDelay();
    I2C_SCL = 1;
    I2CLDelay();
    I2C_SDA = 0;
    I2CLDelay();
    I2CLDelay();
    I2CLDelay();
    I2C_SCL = 0;//pull down SDA when SCL is high.
    I2CLDelay();
    I2CLDelay();
    I2CLDelay();
    return I2CLSend((addr << 1) | rw);
}

void I2CLInit(){
    I2C_SCL = 1;
    I2C_SDA = 1;
}

void I2CLSendData(uint8 xdata *dat,uint16 len){
    uint16 i;
    for (i=0 ; i<len ; i++){
        I2CLSend(dat[i]);
    }
}