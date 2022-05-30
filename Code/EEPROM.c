#include "EEPROM.h"
#include "STC15W.h"
#include "INTRINS.H"

#define CMD_STANDBY 0
#define CMD_READ    1
#define CMD_WRITE   2
#define CMD_ERASE    3

void EEPROMEnable(){
    IAP_TRIG = 0;
    IAP_CONTR |= 0x80;
}

void EEPROMDisable(){
    IAP_TRIG = 0;
    IAP_CONTR &= 0x7F;
}

void EEPROMWrite(uint16 addr,uint8 dat){
    IAP_ADDRH = addr >> 8;
    IAP_ADDRL = addr;
    IAP_CMD = CMD_WRITE;
    IAP_DATA = dat;
    IAP_TRIG = 0x5A;
    IAP_TRIG = 0xA5;
}

uint8 EEPROMRead(uint16 addr,uint8 dat){
    IAP_ADDRH = addr >> 8;
    IAP_ADDRL = addr;
    IAP_CMD = CMD_READ;
    IAP_TRIG = 0x5A;
    IAP_TRIG = 0xA5;
    _nop_();
    return IAP_DATA;
}

void EEPROMErase(uint16 addr){
    IAP_ADDRH = addr >> 8;
    IAP_ADDRL = addr;
    IAP_CMD = CMD_ERASE;
    IAP_TRIG = 0x5A;
    IAP_TRIG = 0xA5;
}