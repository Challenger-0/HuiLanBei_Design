#include "dataTypeDef.h"

void UARTInit(void);

void UARTDBWrite(uint8 dat);
uint8 UARTDBAvaliable();
uint8 UARTDBRead();
void UARTDBSendStr(char *str);

void UARTMPWrite(uint8 dat); //向缓冲区写一字节
uint8 UARTMPAvaliable();
uint8 UARTMPRead(); //从缓冲区读一字节

void UARTBTWrite(uint8 dat);
uint8 UARTBTAvaliable();
uint8 UARTBTRead();