#include "UART.h"
#include "MP3Player.h"

#define MP3PLAYER_VERSION 0xff


#define MP3_CMD_PLAY_TRACK_UNDER_MP3 0x12

void MP3PlayerSendCommand(uint8 cmd,uint8 para1,uint8 para2){
    uint16 checkSum = 0xffff - MP3PLAYER_VERSION - 0x06 - cmd - 0x00 - para1 - para2 + 1;
    UARTMPWrite(0x7E);//起始位
    UARTMPWrite(MP3PLAYER_VERSION);//版本
    UARTMPWrite(0x06);//命令长度
    UARTMPWrite(cmd);//命令
    UARTMPWrite(0x00);//不需要返回值
    UARTMPWrite(para1);//参数1
    UARTMPWrite(para2);//参数2
    UARTMPWrite(checkSum >> 8);//校验和高8位
    UARTMPWrite(checkSum);//校验和低8位
    UARTMPWrite(0xef);//结束位

    UARTDBWrite(0x7E);//起始位
    UARTDBWrite(MP3PLAYER_VERSION);//版本
    UARTDBWrite(0x06);//命令长度
    UARTDBWrite(cmd);//命令
    UARTDBWrite(0x00);//不需要返回值
    UARTDBWrite(para1);//参数1
    UARTDBWrite(para2);//参数2
    UARTDBWrite(checkSum >> 8);//校验和高8位
    UARTDBWrite(checkSum);//校验和低8位
    UARTDBWrite(0xef);//结束位
}

void MP3PlayerPlayTrack(uint16 track){
    MP3PlayerSendCommand(MP3_CMD_PLAY_TRACK_UNDER_MP3, track >> 8, track);
}

void MP3PlayerInit(){
    MP3PlayerSendCommand(0x09, 0x00, 0x02);//SD卡播放
}