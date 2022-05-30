#include "WS2812.h"
#include "INTRINS.H"

const uint8 code WS2812DataByteNum = LED_NUM * 3;
const uint8 code WS2812DataMask[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

void WS2812Init(){
    P3M0 &= ~(1 << 5);
    P3M1 &= ~(1 << 5);//准双向
    WS2812_DAT = 0;
}

void WS2812Update(color *d){//
    //这种写法按理说是错的，没有精确调过时序，但是既然他能跑起来...嘿嘿嘿,那也不在乎了
    uint8 *dat = (uint8 *)d, i, j,EAStatus=EA;
    EA = 0;
    for (i = 0; i < WS2812DataByteNum; ++i){
        for (j = 0; j < 8; ++j){
            if(*dat&WS2812DataMask[j]){
                WS2812_DAT = 1;
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                WS2812_DAT = 0;

            }
            else{
                WS2812_DAT=1;
                _nop_();
                _nop_();
                _nop_();
                _nop_();
                WS2812_DAT = 0;
            }
        }
        ++dat;
    }
    EA = EAStatus;
    // WS2812_DAT = 0;
    // _nop_();
    // _nop_();
    // _nop_();
    // _nop_();
    // _nop_();
    // _nop_();
    // _nop_();
    // _nop_();
    // _nop_();
    // _nop_();
    // _nop_();
    // _nop_();
    // _nop_();
    // _nop_();
    
}
