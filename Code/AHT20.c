#include "AHT20.h"
#include "I2C_low_speed.h"

#define AHT20_ADDR 0x38

extern uint32 xdata millis;

humiture *AHT20DataAddr;//返回数据的地址

uint8 AHT20IsReading = false;

#define AHT20_START_READING   0
#define AHT20_WAIT_FOR_RESULT 1
#define AHT20_GET_RESULT      2
#define AHT20_COLD_DOWN       3//冷却

uint32 AHT20StartReadTime,AHT20EndReadTime;//记录开始读的时间，用于把控延时

uint8 AHT20ReadingStep;

uint32 AHT20HumiRaw, AHT20TempRaw;

void getHumiture(humiture *dat){//注意，不会立刻返回值，读取需要时间，读取完成后数据会送回传入的地址
    if(!AHT20IsReading){
        AHT20IsReading = true;
        AHT20DataAddr = dat;
        AHT20ReadingStep = 0;
    }
}

void AHT20Handle(){//放在loop里，反复执行即可，不然就用延时卡进程:(
    uint8 tmp;
    uint32 tmp2;
    static uint8 xdata AHT20Data[5];
    if(AHT20IsReading){
        switch(AHT20ReadingStep){
            case AHT20_START_READING:
                I2CLStart(AHT20_ADDR, I2C_WRITE);
                I2CLSend(0xac);
                I2CLSend(0x33);
                I2CLSend(0x00);
                I2CLEnd();
                AHT20StartReadTime = millis;
                ++AHT20ReadingStep;
                break;
            case AHT20_WAIT_FOR_RESULT:
                if(millis-AHT20StartReadTime>10)
                    ++AHT20ReadingStep;
                break;
            case AHT20_GET_RESULT:
                I2CLStart(AHT20_ADDR, I2C_READ);
                tmp = I2CLReceive(I2C_NAK);
                if(!(tmp&0x80)){
                    AHT20Data[0] = I2CLReceive(I2C_ACK);
                    AHT20Data[1] = I2CLReceive(I2C_ACK);
                    AHT20Data[2] = I2CLReceive(I2C_ACK);
                    AHT20Data[3] = I2CLReceive(I2C_ACK);
                    AHT20Data[4] = I2CLReceive(I2C_NAK);//不校验
                    tmp2 = 0;
                    tmp2 = (tmp2 | AHT20Data[0]) << 8;
                    tmp2 = (tmp2 | AHT20Data[1]) << 8;
                    tmp2 |= AHT20Data[2];
                    tmp2 >>= 4;
                    AHT20HumiRaw = tmp2;
                    tmp2 = 0;
                    tmp2 = (tmp2 | AHT20Data[2]) << 8;
                    tmp2 = (tmp2 | AHT20Data[3]) << 8;
                    tmp2 |= AHT20Data[4];
                    tmp2 &= 0xfffff;
                    AHT20TempRaw = tmp2;
                    AHT20DataAddr->humi = AHT20HumiRaw * 100 / 1024 / 1024;
                    AHT20DataAddr->temp = AHT20TempRaw * 200 / 1024 / 1024;
                    AHT20EndReadTime = millis;
                    ++AHT20ReadingStep;
                }
                I2CLEnd();
                break;
            case AHT20_COLD_DOWN:
                if(millis-AHT20EndReadTime>1000){
                    AHT20IsReading = false;
                    AHT20ReadingStep = 0;
                }
                break;
        }
    }
}

void AHT20Init(){
    I2CLStart(AHT20_ADDR, I2C_WRITE);
    I2CLSend(0xa8);
    I2CLSend(0x00);
    I2CLSend(0x00);
    I2CLEnd();

    I2CLStart(AHT20_ADDR, I2C_WRITE);
    I2CLSend(0xbe);
    I2CLSend(0x08);
    I2CLSend(0x00);
    I2CLEnd();
}