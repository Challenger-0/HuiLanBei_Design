#include "dataTypeDef.h"

typedef struct {
     char *title;
     char code **item;
     uint8  itemNum;
     uint8  *selection;
} GUIListData;

typedef struct {
    char *title;
    int16 *max;
    int16 *min;
    int16 *val;
    int16 *step;
} GUINumberSelectData;

typedef struct {
    uint16 total;
    uint16 left;
    uint8 running;
} GUICountdownData;

void drawMain(int8 x, int8 y, time *t);
void drawHumiture(int8 x,int8 y, humiture *dat);
void drawList(int8 x, int8 y, GUIListData *dat);
void drawNumberSelect(int8 x, int8 y, GUINumberSelectData *dat);
void drawCountdown(int8 x,int8 y,GUICountdownData *dat);
void drawAlarm(int8 x, int8 y, alarmData *dat);
void drawText(int8 x, int8 y, char *text);