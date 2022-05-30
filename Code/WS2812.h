#ifndef WS2812_H
#define WS2812_H
#include"STC15W.h"
#include "dataTypeDef.h"
#define WS2812_DAT P35
#define LED_NUM 6 //LED的数量

void WS2812Update(color *d);
void WS2812Init();
#endif // !WS2812_H
