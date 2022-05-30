#include "graphics.h"
#include "SSD1306.h"
#include "INTRINS.H"

uint8 code FONT_DATA[57][3] = {
    {0XCF,0X30,0X60},//0 0
    {0X8C,0XC0,0X00},//1 1
    {0XFD,0X20,0X00},//2 2
    {0XFC,0X30,0X00},//3 3
    {0x32,0x30,0x00},//4 4
    {0XFE,0X10,0X00},//5 5
    {0XFF,0X10,0X00},//6 6
    {0XC0,0X30,0X00},//7 7
    {0XFF,0X30,0X00},//8 8
    {0XFE,0X30,0X00},//9 9
    {0X31,0X14,0X80},//A 10
    {0XCF,0X01,0X40},//B 11
    {0XCF,0X00,0X00},//C 12
    {0X8B,0X04,0X10},//D 13
    {0XFF,0X00,0X00},//E 14
    {0XF3,0X00,0X00},//F 15
    {0XDF,0X10,0X00},//G 16
    {0X33,0X30,0X00},//H 17
    {0XCC,0XC0,0X00},//I 18
    {0X44,0X32,0X00},//J 19
    {0X23,0X01,0X40},//K 20
    {0X0F,0X00,0X00},//L 21
    {0X03,0X38,0X40},//M 22
    {0X03,0X39,0X00},//N 23
    {0XCF,0X30,0X00},//O 24
    {0XF3,0X20,0X00},//P 25
    {0XCF,0X31,0X00},//Q 26
    {0XF3,0X21,0X00},//R 27
    {0XFE,0X10,0X00},//S 28
    {0XC0,0XC0,0X00},//T 29
    {0X0F,0X30,0X00},//U 30
    {0X02,0X22,0X10},//V 31
    {0X03,0X31,0X20},//W 32
    {0X00,0X09,0X60},//X 33
    {0X00,0X48,0X40},//Y 34
    {0XCC,0X00,0X60},//Z 35
    {0X50,0X80,0X08},//℃ 36
    {0X00,0X00,0X69},//% 37
    {0X30,0XC0,0X00},//+ 38
    {0X30,0X00,0X00},//- 39
    {0XC0,0X00,0X00},//_ 40
    {0X00,0X00,0X06},//: 41
    {0X00,0X00,0X60},/// 42
    {0X00,0X09,0X00},//\ 43
    {0X00,0X00,0X02},//. 44
    {0X00,0X80,0X02},//! 45
    {0X44,0XC0,0X00},//[ 46
    {0X88,0XC0,0X00},//] 47
    {0X00,0XC0,0X00},//| 48
    {0X00,0X01,0X40},//< 49
    {0X00,0X08,0X20},//> 50
    {0XD0,0X20,0X02},//? 51
    {0X64,0XC0,0X00},//{ 52
    {0X98,0XC0,0X00},//} 53
    {0X00,0X04,0X80},//^ 54
    {0X00,0X00,0X00},//Space 55
    {0XCF,0X39,0X60}//Default 56
};

uint8 code ASCII_MAP[] = {
    56,//0
    56,//1
    56,//2
    56,//3
    56,//4
    56,//5
    56,//6
    56,//7
    56,//8
    55,//9
    56,//10
    56,//11
    56,//12
    56,//13
    56,//14
    56,//15
    56,//16
    56,//17
    56,//18
    56,//19
    56,//20
    56,//21
    56,//22
    56,//23
    56,//24
    56,//25
    56,//26
    56,//27
    56,//28
    56,//29
    56,//30
    56,//31
    55,//32 Space
    45,//33 !
    56,//34 
    56,//35 
    56,//36 
    37,//37
    56,//38
    56,//39
    46,//40 (
    47,//41 )
    56,//42
    38,//43
    56,//44
    39,//45 -
    44,//46 .
    42,//47 /
    0, //48 0
    1, //49 1
    2, //50 2
    3, //51 3
    4, //52 4
    5, //53 5
    6, //54 6
    7, //55 7
    8, //56 8
    9, //57 9
    41,//58 :
    56,//59
    49,//60 <
    56,//61
    50,//62
    51,//63
    56,//64
    10,//65
    11,//66
    12,//67
    13,//68
    14,//69
    15,//70
    16,//71
    17,//72
    18,//73
    19,//74
    20,//75
    21,//76
    22,//77
    23,//78
    24,//79
    25,//80
    26,//81
    27,//82
    28,//83
    29,//84
    30,//85
    31,//86
    32,//87
    33,//88
    34,//89
    35,//90
    46,//91 [
    43,//92 \'
    47,//93 ]
    54,//94 ^
    40,//95 _
    36,//96 `->℃
    10,//97
    11,//98
    12,//99
    13,//100
    14,//101
    15,//102
    16,//103
    17,//104
    18,//105
    19,//106
    20,//107
    21,//108
    22,//109
    23,//110
    24,//111
    25,//112
    26,//113
    27,//114
    28,//115
    29,//116
    30,//117
    31,//118
    32,//119
    33,//120
    34,//121
    35,//122
    52,//123
    48,//124
    53,//125
    56,//126
    56 //127
};

uint8 xdata graphicsBuffer[8][128]; //显存

uint8 drawColor = 1;//0黑1白2异或

uint8 fontColor = 1;//0黑1白

uint8 fontWeight = 1, fontSpacing = 1, fontHeight=3, fontWidth=2;

void graphicsInit(){
    uint16 i;
    OLEDInit();
    for (i = 0; i < 1024;i++)
        (*graphicsBuffer)[i] = 0x00;
    graphicsUpdate();
}

void graphicsUpdate(){
    OLEDDataPointerReset();
    OLEDSendDataPackage(*graphicsBuffer, 1024);
}

void drawPixel(int8 x,int8 y){
    int8 page = y / 8, pageBit, dat;
    if(x<0||y<0)
        return;
    pageBit = y % 8;
    dat = 1 << pageBit;
    if (drawColor==COLOR_0)
        graphicsBuffer[y/8][x] &= ~dat;
    else if (drawColor==COLOR_1)
        graphicsBuffer[y/8][x] |= dat;
    else
        graphicsBuffer[y/8][x] |= dat;
}

void drawHLine(int8 x,int8 y, uint8 len){
    int8 xend=x+len, page = y / 8, dat;
    if(y<0 || x<0&&xend<0)
        return;
    if (drawColor==0){
        dat = ~(1 << y%8);
        for (; x != xend; ++x){
            if(x<0)
                continue;
            graphicsBuffer[page][x] &= dat;
        }
    }
    else if (drawColor==1){
        dat = 1 << y%8;
        for (; x != xend && x>=0 ; ++x){
            if(x<0)
                continue;
            graphicsBuffer[page][x] |= dat;
        }
    }
    else{
        dat = 1 << y%8;
        for (; x != xend && x>=0 ; ++x){
            if(x<0)
                continue;
            graphicsBuffer[page][x] ^= dat;
        }
    }
}

void drawVLine(int8 x,int8 y, uint8 len){
    int8 startPage = y / 8, endPage = (y + len) / 8, startPageBit = y % 8, endPageBit = (y + len) % 8, startData, endData,page;
    if(x<0)
        return;
    if(startPage==endPage){
        if(startPage>=0){
            startData = 0xFF & (0xFF << startPageBit) & ~(0xff<<endPageBit);
            if (drawColor==COLOR_0)
                graphicsBuffer[startPage][x] &= ~startData;
            else if(drawColor==COLOR_1)
                graphicsBuffer[startPage][x] |= startData;
            else
                graphicsBuffer[startPage][x] ^= startData;
        }
    }
    else{
        startData = 0xFF << startPageBit;
        endData = ~(0xff << endPageBit);
        if (drawColor ==COLOR_0){
            if(startPage>=0) graphicsBuffer[startPage][x] &= ~startData;
            for (page = startPage + 1; page < endPage; ++page){
                if(page>=0)graphicsBuffer[page][x] = 0x00;
            }
            if(endPage<8 && endPage>=0)graphicsBuffer[endPage][x] &= ~endData;
        }
        else if (drawColor==COLOR_1){
            if(startPage>=0) graphicsBuffer[startPage][x] |= startData;
            for (page = startPage + 1; page < endPage && page < 8 &&page>=0; ++page){
                graphicsBuffer[page][x] = 0xFF;
            }
            if(endPage<8 && endPage>=0)graphicsBuffer[endPage][x] |= endData;
        }
        else {
            if(startPage>=0) graphicsBuffer[startPage][x] ^= startData;
            for (page = startPage + 1; page < endPage && page < 8 &&page>=0; ++page){
                graphicsBuffer[page][x] ^= 0xFF;
            }
            if(endPage<8 && endPage>=0)graphicsBuffer[endPage][x] ^= endData;
        }
    }
}

void drawBox(int8 x,int8 y,uint8 width,uint8 height,uint8 filled){
    int8 xEnd = x + width;
    if(filled){
        for (; x != xEnd;x++){
            if (x<0)
                continue;
            drawVLine(x, y, height);
        }
    }
    else{
        drawHLine(x, y, width);
        drawVLine(x, y+1, height-2);
        drawHLine(x, y + height - 1, width);
        drawVLine(x + width - 1, y+1, height-2);
    }
}

void draw45Line(int8 x,int8 y ,uint8 len){
    int8 xend=x+len, page = y / 8, pageBit= y%8 ,dat;
    if (drawColor==COLOR_0){
        dat = ~ (1 << pageBit);
        for (; x != xend && page<8 ;x++){
            if (x<0)
                continue;
            graphicsBuffer[page][x] &= dat;
            dat=_crol_(dat, 1);
            ++pageBit;
            if(pageBit==8){
                pageBit = 0;
                ++page;
            }
        }
    }
    else if (drawColor==COLOR_1){
        dat = 1 << pageBit;
        for (; x != xend && page<8 ;x++){
            if (x<0)
                continue;
            graphicsBuffer[page][x] |= dat;
            dat=_crol_(dat, 1);
            ++pageBit;
            if(pageBit==8){
                pageBit = 0;
                ++page;
            }
        }
    }
    else {
        dat = 1 << pageBit;
        for (; x != xend && page<8 ;x++){
            if (x<0)
                continue;
            graphicsBuffer[page][x] ^= dat;
            dat=_crol_(dat, 1);
            ++pageBit;
            if(pageBit==8){
                pageBit = 0;
                ++page;
            }
        }
    }

}

void draw135Line(int8 x,int8 y ,uint8 len){
    int8 xend=x-len, page = y / 8, pageBit= y%8 ,dat;
    if (drawColor==COLOR_0){
        dat = ~ (1 << pageBit);
        for (; x != xend && page<8 ;x--){
            if (x<0)
                continue;
            graphicsBuffer[page][x] &= dat;
            dat=_crol_(dat, 1);
            ++pageBit;
            if(pageBit==8){
                pageBit = 0;
                ++page;
            }
        }
    }
    else if (drawColor==COLOR_1){
        dat = 1 << pageBit;
        for (; x != xend && page<8 ;x--){
            if (x<0)
                continue;
            graphicsBuffer[page][x] |= dat;
            dat=_crol_(dat, 1);
            ++pageBit;
            if(pageBit==8){
                pageBit = 0;
                ++page;
            }
        }
    }
    else {
        dat = 1 << pageBit;
        for (; x != xend && page<8 ;x--){
            if (x<0)
                continue;
            graphicsBuffer[page][x] ^= dat;
            dat=_crol_(dat, 1);
            ++pageBit;
            if(pageBit==8){
                pageBit = 0;
                ++page;
            }
        }
    }
}

void setDrawColor(uint8 color){
    drawColor = color;
}

void setFontSize(uint8 size){
    fontHeight = size;
    fontWidth = size;
}

void setFontWidth(uint8 size){
    fontWidth = size;
}

void setFontHeight(uint8 size){
    fontHeight = size;
}

void setFontWeight(uint8 weight){
    fontWeight = weight;
}

void setFontSpacing(uint8 spacing){
    fontSpacing = spacing;
}

void drawCirclePolt(int8 x, int8 y, int8 x0, int8 y0)
{
      drawPixel(x0 + x, y0 - y);
      drawPixel(x0 + y, y0 - x);
      drawPixel(x0 - x, y0 - y);
      drawPixel(x0 - y, y0 - x);
      drawPixel(x0 + x, y0 + y);
      drawPixel(x0 + y, y0 + x);
      drawPixel(x0 - x, y0 + y);
      drawPixel(x0 - y, y0 + x);
}

void drawCircleFrame(int8 x0, int8 y0, uint8 rad)
{
    int8 f;
    int8 ddF_x;
    int8 ddF_y;
    uint8 x;
    uint8 y;

    f = 1;
    f -= rad;
    ddF_x = 1;
    ddF_y = 0;
    ddF_y -= rad;
    ddF_y *= 2;
    x = 0;
    y = rad;

    drawCirclePolt(x, y, x0, y0);
    
    while ( x < y )
    {
      if (f >= 0) 
      {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x;

      drawCirclePolt(x, y, x0, y0);    
    }
}

void drawDiscPart(int8 x, int8 y, int8 x0, int8 y0)
{
      drawVLine(x0+x, y0-y, y+1);
      drawVLine(x0+y, y0-x, x+1);
      drawVLine(x0-x, y0-y, y+1);
      drawVLine(x0-y, y0-x, x+1);
      drawVLine(x0+x, y0, y+1);
      drawVLine(x0+y, y0, x+1);
      drawVLine(x0-x, y0, y+1);
      drawVLine(x0-y, y0, x+1);
}

void drawDisc(int8 x0, int8 y0, uint8 rad)
{
  int8 f;
  int8 ddF_x;
  int8 ddF_y;
  uint8 x;
  uint8 y;

  f = 1;
  f -= rad;
  ddF_x = 1;
  ddF_y = 0;
  ddF_y -= rad;
  ddF_y *= 2;
  x = 0;
  y = rad;

  drawDiscPart(x, y, x0, y0);
  
  while ( x < y )
  {
    if (f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawDiscPart(x, y, x0, y0);
  }
}

void drawCircle(int8 x,int8 y,uint8 r, uint8 filled){
    if(r==0){
        drawPixel(x, y);
        return;
    }
    if(filled){
        drawDisc(x, y, r);
    }
    else{
        drawCircleFrame(x, y, r);
    }
}

void drawChar(int8 x, int8 y, int8 ch){
    uint8 data0 = FONT_DATA[ASCII_MAP[ch]][0], data1 = FONT_DATA[ASCII_MAP[ch]][1], data2 = FONT_DATA[ASCII_MAP[ch]][2];
    uint8 colorOrigin = drawColor, i, HLineLen = fontWidth + 1, VLineLen = fontHeight + 1;
    int8 x0 = x, x1 = x + fontWidth, x2 = x + 2 * fontWidth, y0 = y, y1 = y + fontHeight, y2 = y + 2 * fontHeight;
    setDrawColor(fontColor);
    if(data0&0x80){//1
        drawHLine(x0, y0, HLineLen);
        for (i = 1; i < fontWeight;i++){
            drawHLine(x0, y0+i, HLineLen);
            drawHLine(x0, y0-i, HLineLen);
        }
    }
    if(data0&0x40){//2
        drawHLine(x1, y0, HLineLen);
        for (i = 1; i < fontWeight; i++){
            drawHLine(x1, y0 + i, HLineLen);
            drawHLine(x1, y0 - i, HLineLen);
        }
    }
    if(data0&0x20){//3
        drawHLine(x0, y1, HLineLen);
        for (i = 1; i < fontWeight;i++){
            drawHLine(x0, y1+i, HLineLen);
            drawHLine(x0, y1-i, HLineLen);
        }
    }
    if(data0&0x10){//4
        drawHLine(x1, y1, HLineLen);
        for (i = 1; i < fontWeight;i++){
            drawHLine(x1, y1+i, HLineLen);
            drawHLine(x1, y1-i, HLineLen);
        }
    }
    if(data0&0x08){//5
        drawHLine(x0, y2, HLineLen);
        for (i = 1; i < fontWeight;i++){
            drawHLine(x0, y2+i, HLineLen);
            drawHLine(x0, y2-i, HLineLen);
        }
    }
    if(data0&0x04){//6
        drawHLine(x1, y2, HLineLen);
        for (i = 1; i < fontWeight;i++){
            drawHLine(x1, y2+i, HLineLen);
            drawHLine(x1, y2-i, HLineLen);
        }
    }
    if(data0&0x02){//7
        drawVLine(x0, y0, VLineLen);
        for (i = 1; i < fontWeight;i++){
            drawVLine(x0+i, y0, VLineLen);
            drawVLine(x0-i, y0, VLineLen);
        }
    }
    if(data0&0x01){//8
        drawVLine(x0, y1, VLineLen);
        for (i = 1; i < fontWeight;i++){
            drawVLine(x0+i, y1, VLineLen);
            drawVLine(x0-i, y1, VLineLen);
        }
    }

    if(data1&0x80){//1
        drawVLine(x1, y0, VLineLen);
        for (i = 1; i < fontWeight;i++){
            drawVLine(x1+i, y0, VLineLen);
            drawVLine(x1-i, y0, VLineLen);
        }
    }
    if(data1&0x40){//2
        drawVLine(x1, y1, VLineLen);
        for (i = 1; i < fontWeight;i++){
            drawVLine(x1+i, y1, VLineLen);
            drawVLine(x1-i, y1, VLineLen);
        }
    }
    if(data1&0x20){//3
        drawVLine(x2, y0, VLineLen);
        for (i = 1; i < fontWeight;i++){
            drawVLine(x2+i, y0, VLineLen);
            drawVLine(x2-i, y0, VLineLen);
        }
    }
    if(data1&0x10){//4
        drawVLine(x2, y1, VLineLen);
        for (i = 1; i < fontWeight;i++){
            drawVLine(x2+i, y1, VLineLen);
            drawVLine(x2-i, y1, VLineLen);
        }
    }
    if(data1&0x08){//5
        drawLine(x0, y0, x1, y1);
        for (i = 1; i < fontWeight;i++){
            drawLine(x0+i, y0, x1+i, y1);
            drawLine(x0-i, y0, x1-i, y1);
        }
    }
    if(data1&0x04){//6
        drawLine(x1, y0, x2,y1);
        for (i = 1; i < fontWeight;i++){
            drawLine(x1+i, y0, x2+i,y1);
            drawLine(x1-i, y0, x2-i,y1);
        }
    }
    if(data1&0x02){//7
        drawLine(x0, y1, x1,y2);
        for (i = 1; i < fontWeight;i++){
            drawLine(x0+i, y1, x1+i,y2);
            drawLine(x0-i, y1, x1-i,y2);
        }
    }
    if(data1&0x01){//8
        drawLine(x1, y1, x2,y2);
        for (i = 1; i < fontWeight;i++){
            drawLine(x1+i, y1, x2+i,y2);
            drawLine(x1-i, y1, x2-i,y2);
        }
    }

    if(data2&0x80){
        drawLine(x1, y0, x0,y1);
        for (i = 1; i < fontWeight;i++){
            drawLine(x1+i, y0, x0+i,y1);
            drawLine(x1-i, y0, x0-i,y1);
        }
    }
    if(data2&0x40){
        drawLine(x2, y0, x1,y1);
        for (i = 1; i < fontWeight;i++){
            drawLine(x2+i, y0, x1+i,y1);
            drawLine(x2-i, y0, x1-i,y1);
        }
    }
    if(data2&0x20){
        drawLine(x1, y1, x0,y2);
        for (i = 1; i < fontWeight;i++){
            drawLine(x1+i, y1, x0+i,y2);
            drawLine(x1-i, y1, x0-i,y2);
        }
    }   
    if(data2&0x10){
        drawLine(x2, y1, x1,y2);
        for (i = 1; i < fontWeight;i++){
            drawLine(x2+i, y1, x1+i,y2);
            drawLine(x2-i, y1, x1-i,y2);
        }
    }
    if(data2&0x08)
        drawCircle(x0, y0, fontWeight-1, FILLED);
    if (data2 & 0x04)
        drawCircle(x1, y1, fontWeight-1, FILLED);
    if(data2&0x02)
        drawCircle(x1, y2, fontWeight-1, FILLED);
    if(data2&0x01)
        drawCircle(x2, y2, fontWeight-1, FILLED);
    setDrawColor(colorOrigin);
}

void drawStr(int8 x,int8 y,char *str){
    uint8 i = 0;
    while(str[i]){
        drawChar(x, y, str[i]);
        x += 2 * (fontWidth + fontWeight)+fontSpacing-1;
        ++i;
    }
}

void drawChar90Deg(int8 x, int8 y, int8 ch){
    uint8 data0 = FONT_DATA[ASCII_MAP[ch]][0], data1 = FONT_DATA[ASCII_MAP[ch]][1], data2 = FONT_DATA[ASCII_MAP[ch]][2];
    uint8 colorOrigin = drawColor, i, HLineLen = fontHeight + 1, VLineLen = fontWidth + 1;
    int8 x0 = x, x1 = x + fontHeight, x2 = x + 2 * fontHeight, y0 = y, y1 = y - fontWidth, y2 = y - 2 * fontWidth;
    setDrawColor(fontColor);
    if(data0&0x80){//1
        drawVLine(x0, y1, VLineLen);
        for (i = 1; i < fontWeight;i++){
            drawVLine(x0+i, y1, VLineLen);
            drawVLine(x0+i, y1, VLineLen);
        }
    }
    if(data0&0x40){//2
        drawVLine(x0, y2, VLineLen);
        for (i = 1; i < fontWeight; i++){
            drawVLine(x0+i, y2, VLineLen);
            drawVLine(x0-i, y2, VLineLen);
        }
    }
    if(data0&0x20){//3
        drawVLine(x1, y1, VLineLen);
        for (i = 1; i < fontWeight;i++){
            drawVLine(x1+i, y1, VLineLen);
            drawVLine(x1-i, y1, VLineLen);
        }
    }
    if(data0&0x10){//4
        drawVLine(x1, y2, VLineLen);
        for (i = 1; i < fontWeight;i++){
            drawVLine(x1+i, y2, VLineLen);
            drawVLine(x1-i, y1-i, VLineLen);
        }
    }
    if(data0&0x08){//5
        drawVLine(x2, y1, VLineLen);
        for (i = 1; i < fontWeight;i++){
            drawVLine(x2+i, y1, VLineLen);
            drawVLine(x2-i, y1, VLineLen);
        }
    }
    if(data0&0x04){//6
        drawVLine(x2, y2, VLineLen);
        for (i = 1; i < fontWeight;i++){
            drawVLine(x2+i, y2, VLineLen);
            drawVLine(x2-i, y2, VLineLen);
        }
    }
    if(data0&0x02){//7
        drawHLine(x0, y0, HLineLen);
        for (i = 1; i < fontWeight;i++){
            drawHLine(x0, y0+i, HLineLen);
            drawHLine(x0, y0-i, HLineLen);
        }
    }
    if(data0&0x01){//8
        drawHLine(x1, y0, HLineLen);
        for (i = 1; i < fontWeight;i++){
            drawHLine(x1, y0+i, HLineLen);
            drawHLine(x1, y0-i, HLineLen);
        }
    }

    if(data1&0x80){//1
        drawHLine(x0, y1, HLineLen);
        for (i = 1; i < fontWeight;i++){
            drawHLine(x0, y1+i, HLineLen);
            drawHLine(x0, y1-i, HLineLen);
        }
    }
    if(data1&0x40){//2
        drawHLine(x1, y1, HLineLen);
        for (i = 1; i < fontWeight;i++){
            drawHLine(x1, y1+i, HLineLen);
            drawHLine(x1, y1-i, HLineLen);
        }
    }
    if(data1&0x20){//3
        drawHLine(x0, y2, HLineLen);
        for (i = 1; i < fontWeight;i++){
            drawHLine(x0, y2+i, HLineLen);
            drawHLine(x0, y2-i, HLineLen);
        }
    }
    if(data1&0x10){//4
        drawHLine(x1, y2, HLineLen);
        for (i = 1; i < fontWeight;i++){
            drawHLine(x1, y2+i, HLineLen);
            drawHLine(x1, y2-i, HLineLen);
        }
    }
    if(data1&0x08){//5
        drawLine(x0, y0, x1, y1);
        for (i = 1; i < fontWeight;i++){
            drawLine(x0, y0+i, x1, y1+i);
            drawLine(x0, y0-i, x1, y1-i);
        }
    }
    if(data1&0x04){//6
        drawLine(x0, y1, x1,y2);
        for (i = 1; i < fontWeight;i++){
            drawLine(x0, y1+i, x1,y2+i);
           drawLine(x0, y1-i, x1,y2-i);
        }
    }
    if(data1&0x02){//7
        drawLine(x1, y0, x2,y1);
        for (i = 1; i < fontWeight;i++){
            drawLine(x1, y0+i, x2,y1+i);
            drawLine(x1, y0-i, x2,y1-i);
        }
    }
    if(data1&0x01){//8
        drawLine(x1, y1, x2,y2);
        for (i = 1; i < fontWeight;i++){
            drawLine(x1, y1+i, x2,y2+i);
            drawLine(x1, y1-i, x2,y2-i);
        }
    }

    if(data2&0x80){
        drawLine(x0, y1, x1,y0);
        for (i = 1; i < fontWeight;i++){
            drawLine(x0, y1+i, x1,y0+i);
            drawLine(x0, y1-i, x1,y0-i);
        }
    }
    if(data2&0x40){
        drawLine(x0, y2, x1,y1);
        for (i = 1; i < fontWeight;i++){
            drawLine(x0, y2+i, x1,y1+i);
            drawLine(x0, y2-i, x1,y1-i);
        }
    }
    if(data2&0x20){
        drawLine(x1, y1, x2,y0);
        for (i = 1; i < fontWeight;i++){
            drawLine(x1, y1+i, x2,y0-i);
            drawLine(x1, y1+i, x2,y0-i);
        }
    }   
    if(data2&0x10){
        drawLine(x1, y2, x2,y1);
        for (i = 1; i < fontWeight;i++){
            drawLine(x1, y2+i, x2,y1+i);
            drawLine(x1, y2-i, x2,y1-i);
        }
    }
    if(data2&0x08)
        drawCircle(x0, y0, fontWeight-1, FILLED);
    if (data2 & 0x04)
        drawCircle(x1, y1, fontWeight-1, FILLED);
    if(data2&0x02)
        drawCircle(x2, y1, fontWeight-1, FILLED);
    if(data2&0x01)
        drawCircle(x2, y2, fontWeight-1, FILLED);
    setDrawColor(colorOrigin);
}

void drawStr90Deg(int8 x,int8 y,char *str){
    uint8 i = 0;
    while(str[i]){
        drawChar90Deg(x, y, str[i]);
        y -= 2 * (fontWidth + fontWeight)+fontSpacing-1;
        ++i;
    }
}

void drawLine(int8 x1,int8 y1,int8 x2,int8 y2){
 uint8 tmp;
  uint8 x,y;
  uint8 dx, dy;
  int8 err;
  int8 ystep;

  uint8 swapxy = 0;
  
  /* no intersection check at the moment, should be added... */

  if ( x1 > x2 ) dx = x1-x2; else dx = x2-x1;
  if ( y1 > y2 ) dy = y1-y2; else dy = y2-y1;

  if ( dy > dx ) 
  {
    swapxy = 1;
    tmp = dx; dx =dy; dy = tmp;
    tmp = x1; x1 =y1; y1 = tmp;
    tmp = x2; x2 =y2; y2 = tmp;
  }
  if ( x1 > x2 ) 
  {
    tmp = x1; x1 =x2; x2 = tmp;
    tmp = y1; y1 =y2; y2 = tmp;
  }
  err = dx >> 1;
  if ( y2 > y1 ) ystep = 1; else ystep = -1;
  y = y1;

  if ( x2 == 255 )
    x2--;
  for( x = x1; x <= x2; x++ )
  {
    if ( swapxy == 0 ) 
      drawPixel( x, y); 
    else 
      drawPixel( y, x); 
    err -= (uint8)dy;
    if ( err < 0 ) 
    {
      y += (uint8)ystep;
      err += (uint8)dx;
    }
  }
}

void setFontColor(uint8 color){
    fontColor = color;
}

uint8 getStrWidth(char *str){
    uint8 num=0;
    while(str[num])
        num++;
    return ((fontWidth + fontWeight) * 2 -1) * num + fontSpacing * (num - 1);
}

uint8 getFontHeight(){
    return (fontHeight + fontWeight) * 2 -1;
}

uint8 getStrWidthByNum(uint8 num){
    return ((fontWidth + fontWeight) * 2 -1) * num + fontSpacing * (num - 1);
}

uint8 getNumDigital(int16 num){
    uint8 res = 0;
    if(num==0)
        return 1;
    while(num>0){
        num /= 10;
        res++;
    }
    return res;
}

uint16 pow10x(uint8 x){
    uint8 i;
    uint16 res = 1;
    for (i = 0; i < x;i++)
        res *= 10;
    return res;
}

void setContrast(uint8 contrast){
    OLEDSetContrast(contrast);
}