#include "BCD.h"
uint8 BCDDecode(uint8 dat){
    return ((dat >> 4) & 0x0F) * 10 + (dat & 0x0F);
}

uint8 BCDEncode(uint8 dat){
    return (dat / 10) << 4 | (dat % 10);
}