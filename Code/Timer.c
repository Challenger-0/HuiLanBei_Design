#include "Timer.h"

void timer4Init(){
    T4T3M |= 0x20;		//定时器时钟1T模式
	T4L = 0x9A;		//设置定时初始值
	T4H = 0xA9;		//设置定时初始值
	T4T3M |= 0x80;		//定时器4开始计时
    IE2 |= 1 << 6;  //定时器4允许中断
}