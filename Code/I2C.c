#include "I2C.h"

#define I2C_SDA P26
#define I2C_SCL P25



//选择I2C时钟高时延时多少个时钟周期，默认无，最快速率
//...反正占点空间嘛...又不是用不起

#define I2C_DELAY_0_CYCLE


#ifdef I2C_DELAY_0_CYCLE
#define I2CDelay() " "
#else
#ifdef I2C_DELAY_1_CYCLE
#define I2CDelay() _nop_();
#else
#ifdef I2C_DELAY_2_CYCLE
#define I2CDelay() _nop_();_nop_();
#else
#ifdef I2C_DELAY_4_CYCLE
#define I2CDelay() _nop_();_nop_();_nop_();_nop_();
#else
#ifdef I2C_DELAY_8_CYCLE
#define I2CDelay() _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
#else
#ifdef I2C_DELAY_16_CYCLE
#define I2CDelay() _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
#else
#define I2CDelay() " "
#endif
#endif
#endif
#endif
#endif
#endif




uint8 I2CReceive(uint8 ack){
    uint8 dat,EAStatus=EA;

    EA = 0;//禁止一切中断

    I2C_SDA = 1;//释放SDA

    I2C_SCL = 1;
    dat |= I2C_SDA;
    I2CDelay();
    I2C_SCL = 0;
    dat = dat << 1;
    I2CDelay();

    I2C_SCL = 1;
    dat |= I2C_SDA;
    I2CDelay();
    I2C_SCL = 0;
    dat = dat << 1;
    I2CDelay();

    I2C_SCL = 1;
    dat |= I2C_SDA;
    I2CDelay();
    I2C_SCL = 0;
    dat = dat<<1;
    I2CDelay();

    I2C_SCL = 1;
    dat |= I2C_SDA;
    I2CDelay();
    I2C_SCL = 0;
    dat = dat<<1;
    I2CDelay();

    I2C_SCL = 1;
    dat |= I2C_SDA;
    I2CDelay();
    I2C_SCL = 0;
    dat = dat<<1;
    I2CDelay();

    I2C_SCL = 1;
    dat |= I2C_SDA;
    I2CDelay();
    I2C_SCL = 0;
    dat = dat<<1;
    I2CDelay();

    I2C_SCL = 1;
    dat |= I2C_SDA;
    I2CDelay();
    I2C_SCL = 0;
    dat = dat<<1;
    I2CDelay();

    I2C_SCL = 1;
    dat |= I2C_SDA;
    I2CDelay();
    I2C_SCL = 0;
    //dat = dat<<1;
    I2CDelay();

    I2C_SDA = ack;
    I2C_SCL = 1;
    I2CDelay();
    _nop_();
    I2C_SCL = 0;

    EA = EAStatus;
    return dat;
}

uint8 I2CSend(uint8 dat){
    uint8 ack, count,EAStatus=EA;

    EA = 0;//禁用一切中断避免干扰

    I2C_SDA = dat >> 7;//循环的时间都够我跑完了
    I2C_SCL = 1;
    I2CDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CDelay();

    I2C_SDA = dat >> 7;
    I2C_SCL = 1;
    I2CDelay();
    dat <<= 1;
    I2C_SCL = 0;
    I2CDelay();

    I2C_SDA = 1; //release SDA and wait for ack
    I2C_SCL = 1;
    for (count = 16; I2C_SDA && count;count--)
        ;
    ack = I2C_SDA;
    I2C_SCL = 0;

    EA = EAStatus;//恢复中断状态；

    return ack;
}

void I2CEnd(){
    I2C_SDA = 0;
    I2CDelay();
    I2C_SCL = 1;
    I2CDelay();
    I2CDelay();
    I2C_SDA = 1;
}

uint8 I2CStart(uint8 addr,uint8 rw){
    I2C_SDA = 1;
    _nop_();
    I2C_SCL = 1;
    _nop_();
    I2C_SDA = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    I2CDelay();
    I2CDelay();
    I2CDelay();
    I2C_SCL = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    I2CDelay();
    I2CDelay();
    I2CDelay();
    return I2CSend((addr << 1) | rw);
}

void I2CInit(){
    // P2M0 |= 0x60;
    // P2M1 |= 0x60;//开漏
    P2M0 &= ~0x60;
    P2M1 &= ~0x60;//准双向
    I2C_SCL = 1;
    I2C_SDA = 1;
}

void I2CSendData(uint8 xdata *dat,uint16 len){
    uint16 i;
    for (i=0 ; i<len ; i++){
        I2CSend(dat[i]);
    }
}