#ifndef _DATA_TYPE_DEF_
#define _DATA_TYPE_DEF_

#define true 1
#define false 0

#define HIGH 1
#define LOW 0

#define ENABLE 1
#define DISABLE 0


typedef unsigned char  uint8;
typedef char           int8;
typedef unsigned short uint16;
typedef short          int16;
typedef unsigned long  uint32;
typedef long           int32;

typedef struct humiture {
    float humi;
    float temp;
}humiture;

typedef struct time{
    uint8 year;
    uint8 month;
    uint8 day;
    uint8 week;
    uint8 hour;
    uint8 minute;
    uint8 second;
}time;

typedef struct alarmData{
    uint8 enable;
    uint8 hour;
    uint8 minute;
    uint8 ringTone;
}alarmData;

typedef struct color{
    uint8 G;
    uint8 R;
    uint8 B;
} color;

#endif