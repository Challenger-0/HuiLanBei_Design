#include "dataTypeDef.h"
#define COLOR_0   0
#define COLOR_1   1
#define COLOR_XOR 2

#define FILLED    1
#define UNFILLED  0

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

void graphicsInit();
void graphicsUpdate();
void drawPixel(int8 x, int8 y);
void drawHLine(int8 x, int8 y, uint8 len);
void drawVLine(int8 x, int8 y, uint8 len);
void drawBox(int8 x, int8 y, uint8 width, uint8 height, uint8 filled);
void draw45Line(int8 x, int8 y, uint8 len);
void draw135Line(int8 x, int8 y, uint8 len);
void setDrawColor(uint8 color);
void setFontSize(uint8 size);
void setFontHeight(uint8 height);
void setFontWidth(uint8 width);
void setFontWeight(uint8 weight);
void setFontSpacing(uint8 spacing);
void drawCircle(int8 x, int8 y, int8 r, uint8 filled);
void drawStr(int8 x, int8 y, char *str);
void drawStr90Deg(int8 x, int8 y, char *str);
void drawLine(int8 x0, int8 y0, int8 x1, int8 y1);
void setFontColor(uint8 color);
uint8 getStrWidth(char *str);
uint8 getFontHeight();
uint8 getStrWidthByNum(uint8 num);
uint8 getNumDigital(int16 num);
void setContrast(uint8 contrast);
