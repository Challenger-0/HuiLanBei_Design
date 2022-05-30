#include "PCF8563T.h"
#include "I2C_low_speed.h"
#include "BCD.h"

#define PCF8563T_ADDR 0x51
#define ADDR_SEC 0x02
#define ADDR_MIN 0x03
#define ADDR_HR  0x04
#define ADDR_DAY 0x05
#define ADDR_WK  0x06
#define ADDR_MON 0x07
#define ADDR_YR  0x08

uint8 PCF8563ReadByte(uint8 addr){
    uint8 dat;
    I2CLStart(PCF8563T_ADDR, I2C_WRITE);
    I2CLSend(addr);
    I2CLStart(PCF8563T_ADDR, I2C_READ);
    dat = I2CLReceive(I2C_NAK);
    I2CLEnd();
    return dat;
}

void PCF8563WriteByte(uint8 addr,uint8 byte){
    I2CLStart(PCF8563T_ADDR, I2C_WRITE);
    I2CLSend(addr);
    I2CLSend(byte);
    I2CLEnd();
}

void getTime(time xdata *dat){
    // dat->second = BCDDecode(PCF8563ReadByte(ADDR_MIN)&0x7F);
    // dat->minute = BCDDecode(PCF8563ReadByte(ADDR_MIN)&0x7F);
    // dat->hour = BCDDecode(PCF8563ReadByte(ADDR_HR)&0x1F);
    // dat->day = BCDDecode(PCF8563ReadByte(ADDR_DAY)&0x1F);
    // dat->week = BCDDecode(PCF8563ReadByte(ADDR_WK)&0x07);
    // dat->month = BCDDecode(PCF8563ReadByte(ADDR_MON)&0x1F);
    // dat->second = BCDDecode(PCF8563ReadByte(ADDR_YR));
    I2CLStart(PCF8563T_ADDR, I2C_WRITE);
    I2CLSend(ADDR_SEC);
    I2CLStart(PCF8563T_ADDR, I2C_READ);
    dat->second = BCDDecode(I2CLReceive(I2C_ACK) & 0x7F);
    dat->minute = BCDDecode(I2CLReceive(I2C_ACK) & 0x7f);
    dat->hour = BCDDecode(I2CLReceive(I2C_ACK) & 0x3f);
    dat->day = BCDDecode(I2CLReceive(I2C_ACK) & 0x1f);
    dat->week = BCDDecode(I2CLReceive(I2C_ACK) & 0x07);
    dat->month = BCDDecode(I2CLReceive(I2C_ACK) & 0x1f);
    dat->year = BCDDecode(I2CLReceive(I2C_NAK));
    I2CLEnd();
}

void setTime(time xdata *dat){
    I2CLStart(PCF8563T_ADDR, I2C_WRITE);
    I2CLSend(ADDR_SEC);
    I2CLSend(BCDEncode(dat->second));
    I2CLSend(BCDEncode(dat->minute));
    I2CLSend(BCDEncode(dat->hour));
    I2CLSend(BCDEncode(dat->day));
    I2CLSend(BCDEncode(dat->week));
    I2CLSend(BCDEncode(dat->month));
    I2CLSend(BCDEncode(dat->year));
    I2CLEnd();
}