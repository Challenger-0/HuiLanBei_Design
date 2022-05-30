#include "STC15W.h"
#include "UART.h"


#define UART_UPDATE_PASSWORD_LENGTH 12 //不包含\0
const char code UART_UPDATE_PASSWORD[] = "*UARTUPDATE*";
uint8 xdata UARTUpdatePasswordMatch = 0;

void UARTInit(void){		//9600bps@22.1184MHz{
	//UART1
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	ES = 1;//允许串口1中断
	REN = 1;//允许串口1接收
	//UART2
	S2CON = 0x50;		//8位数据,可变波特率
	IE2 |= 0x01;       //允许串口2中断
	S2CON |= 0x10;//允许串口2接收
	//UART3
	S3CON = 0x10;		//8位数据,可变波特率
	S3CON &= 0xBF;		//串口3选择定时器2为波特率发生器
	IE2 |= 0x08;//允许串口3中断
	S3CON |= 0x10;//允许串口3接收

	//Timer2
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0xC0;		//设置定时初始值
	T2H = 0xFD;		//设置定时初始值
	AUXR |= 0x10;		//定时器2开始计时

}

#define UART_DB_W_BUFFER_SIZE 64
#define UART_DB_R_BUFFER_SIZE 64


uint8 xdata UARTDBWriteBuffer[UART_DB_W_BUFFER_SIZE];//循环队列
uint8 xdata UARTDBReadBuffer[UART_DB_R_BUFFER_SIZE];
uint8 UARTDBWritePtr = 0, UARTDBWriteSPtr = 0, UARTDBReadPtr=0, UARTDBReadRPtr=0;
uint8 UARTDBSending=false;

void UARTDBInterrupt() interrupt 4{
	if(RI){//RI
		RI=0;
		UARTDBReadBuffer[UARTDBReadRPtr] = SBUF;
		if(++UARTDBReadRPtr==UART_DB_R_BUFFER_SIZE)
			UARTDBReadRPtr = 0;

		//自动下载部分...
		if(SBUF==UART_UPDATE_PASSWORD[UARTUpdatePasswordMatch]){
			++UARTUpdatePasswordMatch;
			if(UARTUpdatePasswordMatch==UART_UPDATE_PASSWORD_LENGTH)
				IAP_CONTR = 0x60;//软复位到ISP区
		}
		else{
			UARTUpdatePasswordMatch = 0;
		}
		//===================
	}
	if(TI){//TI
		TI=0;
		if(UARTDBSending){
			SBUF = UARTDBWriteBuffer[UARTDBWriteSPtr];//自动发送完剩下的数据
			if(++UARTDBWriteSPtr==UART_DB_W_BUFFER_SIZE)
				UARTDBWriteSPtr = 0;
			if(UARTDBWritePtr==UARTDBWriteSPtr)//发送指针和缓冲指针重合
				UARTDBSending = false;
		}
	}
}

void UARTDBWrite(uint8 dat){//向缓冲区写一字节
	UARTDBWriteBuffer[UARTDBWritePtr] = dat;
	if(++UARTDBWritePtr==UART_DB_W_BUFFER_SIZE)
		UARTDBWritePtr = 0;
	if(!UARTDBSending){//手动送入第一字节开始发送
		UARTDBSending = true;
		SBUF = UARTDBWriteBuffer[UARTDBWriteSPtr];
		if(++UARTDBWriteSPtr==UART_DB_W_BUFFER_SIZE)
			UARTDBWriteSPtr = 0;
	}
}

uint8 UARTDBAvaliable(){
	return UARTDBReadPtr != UARTDBReadRPtr;
}

uint8 UARTDBRead(){//从缓冲区读一字节
	uint8 dat = UARTDBReadBuffer[UARTDBReadPtr];
	if(++UARTDBReadPtr==UART_DB_R_BUFFER_SIZE)
			UARTDBReadPtr = 0;
	return dat;
}

void UARTDBSendStr(char *str){
	uint8 i;
	for (i = 0; str[i];++i){
		UARTDBWrite(str[i]);
	}
}




#define UART_MP_W_BUFFER_SIZE 32 //写缓冲给大点...现在起码能容纳三条指令，永远不知道你的用户会干出什么事
#define UART_MP_R_BUFFER_SIZE 16


uint8 UARTMPWriteBuffer[UART_MP_W_BUFFER_SIZE];//循环队列
uint8 UARTMPReadBuffer[UART_MP_R_BUFFER_SIZE];
uint8 UARTMPWritePtr = 0, UARTMPWriteSPtr = 0, UARTMPReadPtr=0, UARTMPReadRPtr=0;
uint8 UARTMPSending=false;

void UARTMPInterrupt() interrupt 8{
	if(S2CON&0x01){//RI
		S2CON &= 0xFE;
		UARTMPReadBuffer[UARTMPReadRPtr] = S2BUF;
		if(++UARTMPReadRPtr==UART_MP_R_BUFFER_SIZE)
			UARTMPReadRPtr = 0;
	}
	if(S2CON&0x02){//TI
		S2CON &= 0xFD;
		if(UARTMPSending){
			S2BUF = UARTMPWriteBuffer[UARTMPWriteSPtr];//自动发送完剩下的数据
			if(++UARTMPWriteSPtr==UART_MP_W_BUFFER_SIZE)
				UARTMPWriteSPtr = 0;
			if(UARTMPWritePtr==UARTMPWriteSPtr)//发送指针和缓冲指针重合
				UARTMPSending = false;
		}
	}
}

void UARTMPWrite(uint8 dat){//向缓冲区写一字节
	UARTMPWriteBuffer[UARTMPWritePtr] = dat;
	if(++UARTMPWritePtr==UART_MP_W_BUFFER_SIZE)
		UARTMPWritePtr = 0;
	if(!UARTMPSending){//手动送入第一字节开始发送
		UARTMPSending = true;
		S2BUF = UARTMPWriteBuffer[UARTMPWriteSPtr];
		if(++UARTMPWriteSPtr==UART_MP_W_BUFFER_SIZE)
			UARTMPWriteSPtr = 0;
	}
}

uint8 UARTMPAvaliable(){
	return UARTMPReadPtr != UARTMPReadRPtr;
}

uint8 UARTMPRead(){//从缓冲区读一字节
	uint8 dat = UARTMPReadBuffer[UARTMPReadPtr];
	if(++UARTMPReadPtr==UART_MP_R_BUFFER_SIZE)
			UARTMPReadPtr = 0;
	return dat;
}












#define UART_BT_W_BUFFER_SIZE 16
#define UART_BT_R_BUFFER_SIZE 16


uint8 UARTBTWriteBuffer[UART_BT_W_BUFFER_SIZE];//循环队列
uint8 UARTBTReadBuffer[UART_BT_R_BUFFER_SIZE];
uint8 UARTBTWritePtr = 0, UARTBTWriteSPtr = 0, UARTBTReadPtr=0, UARTBTReadRPtr=0;
uint8 UARTBTSending=false;

void UARTBTInterrupt() interrupt 17{
	if(S3CON&0x01){//RI
		S3CON &= 0xFE;
		UARTBTReadBuffer[UARTBTReadRPtr] = S3BUF;
		if(++UARTBTReadRPtr==UART_BT_R_BUFFER_SIZE)
			UARTBTReadRPtr = 0;
	}
	if(S3CON&0x02){//TI
		S3CON &= 0xFD;
		if(UARTBTSending){
			S3BUF = UARTBTWriteBuffer[UARTBTWriteSPtr];//自动发送完剩下的数据
			if(++UARTBTWriteSPtr==UART_BT_W_BUFFER_SIZE)
				UARTBTWriteSPtr = 0;
			if(UARTBTWritePtr==UARTBTWriteSPtr)//发送指针和缓冲指针重合
				UARTBTSending = false;
		}
	}
}

void UARTBTWrite(uint8 dat){//向缓冲区写一字节
	UARTBTWriteBuffer[UARTBTWritePtr] = dat;
	if(++UARTBTWritePtr==UART_BT_W_BUFFER_SIZE)
		UARTBTWritePtr = 0;
	if(!UARTBTSending){//手动送入第一字节开始发送
		UARTBTSending = true;
		S3BUF = UARTBTWriteBuffer[UARTBTWriteSPtr];
		if(++UARTBTWriteSPtr==UART_BT_W_BUFFER_SIZE)
			UARTBTWriteSPtr = 0;
	}
}

uint8 UARTBTAvaliable(){
	return UARTBTReadPtr != UARTBTReadRPtr;
}

uint8 UARTBTRead(){//从缓冲区读一字节
	uint8 dat = UARTBTReadBuffer[UARTBTReadPtr];
	if(++UARTBTReadPtr==UART_BT_R_BUFFER_SIZE)
			UARTBTReadPtr = 0;
	return dat;
}



// #define UART_MP_W_BUFFER_SIZE 16
// #define UART_MP_R_BUFFER_SIZE 16


// uint8 UARTMPWriteBuffer[UART_MP_W_BUFFER_SIZE];//循环队列
// uint8 UARTMPReadBuffer[UART_MP_R_BUFFER_SIZE];
// uint8 UARTMPWritePtr = 0, UARTMPWriteSPtr = 0, UARTMPReadPtr=0, UARTMPReadRPtr=0;
// uint8 UARTMPSending=false;

// void UARTMPInterrupt() interrupt 8{
// 	if(S2CON&0x01){//RI
// 		S2CON &= 0xFE; //RI3=0;
// 		UARTMPReadBuffer[UARTMPReadRPtr] = S2BUF;
// 		if(++UARTMPReadRPtr==UART_MP_R_BUFFER_SIZE)
// 			UARTMPReadRPtr = 0;
// 	}
// 	if(S2CON&0x02){//TI
// 		S2CON &= 0xFD; //TI3=0;
// 		if(UARTMPSending){
// 			S2BUF = UARTMPWriteBuffer[UARTMPWriteSPtr];//自动发送完剩下的数据
// 			if(++UARTMPWriteSPtr==UART_MP_R_BUFFER_SIZE)
// 				UARTMPWriteSPtr = 0;
// 			if(UARTMPWritePtr==UARTMPWriteSPtr){//发送指针和缓冲指针重合
// 				UARTMPSending = false;
// 			}
// 		}
// 	}
// }

// void UARTMPWrite(uint8 dat){//向缓冲区写一字节
// 	uint8 tmp;
// 	UARTMPWriteBuffer[UARTMPWritePtr] = dat;
// 	if(++UARTMPWritePtr==UART_MP_R_BUFFER_SIZE)
// 		UARTMPWritePtr = 0;
// 	if(!UARTMPSending){//手动送入第一字节开始发送
// 		UARTMPSending = true;
// 		tmp = UARTMPWriteBuffer[UARTMPWriteSPtr];
// 		if(++UARTMPWriteSPtr==UART_MP_R_BUFFER_SIZE)
// 			UARTMPWriteSPtr = 0;
// 		S2BUF = tmp;
// 	}
// }

// uint8 UARTMPAvaliable(){
// 	return UARTMPReadPtr != UARTMPReadRPtr;
// }

// uint8 UARTMPRead(){//从缓冲区读一字节
// 	uint8 dat = UARTMPReadBuffer[UARTMPReadPtr];
// 	if(++UARTMPReadPtr==UART_MP_R_BUFFER_SIZE)
// 			UARTMPReadPtr = 0;
// 	return dat;
// }






// #define UART_BT_W_BUFFER_SIZE 16
// #define UART_BT_R_BUFFER_SIZE 16

// uint8 UARTBTWriteBuffer[UART_BT_W_BUFFER_SIZE];
// uint8 UARTBTReadBuffer[UART_BT_R_BUFFER_SIZE];
// uint8 UARTBTWritePtr = 0, UARTBTWriteSPtr = 0, UARTBTReadPtr=0, UARTBTReadRPtr=0;
// uint8 UARTBTSending=false;

// void UARTBTInterrupt() interrupt 17{
// 	if(S3CON&0x01){//RI
// 		S3CON &= 0xFE; //RI3=0;
// 		UARTBTReadBuffer[UARTBTReadRPtr] = S3BUF;
// 		if(++UARTBTReadRPtr==UART_BT_R_BUFFER_SIZE)
// 			UARTBTReadRPtr = 0;
// 	}
// 	if(S3CON&0x02){//TI
// 		S3CON &= 0xFD; //TI3=0;
// 		if(UARTBTSending){
// 			S3BUF = UARTBTWriteBuffer[UARTBTWriteSPtr];//自动发送完剩下的数据
// 			if(++UARTBTWriteSPtr==UART_BT_R_BUFFER_SIZE)
// 				UARTBTWriteSPtr = 0;
// 			if(UARTBTWritePtr==UARTBTWriteSPtr){//发送指针和缓冲指针重合
// 				UARTBTSending = false;
// 			}
// 		}
// 	}
// }

// void UARTBTWrite(uint8 dat){

// }

// uint8 UARTBTAvaliable(){

// }

// uint8 UARTBTRead(){

// }