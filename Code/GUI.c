#include "GUI.h"
#include "graphics.h"
#include "MATH.H"
#include "STDIO.H"

const char code *STR_WEEK[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

void drawMain(int8 x,int8 y,time *t){
    char strTime[6], strDate[6];

    strTime[0] = t->hour / 10 + '0';
    strTime[1] = t->hour % 10 + '0';
    strTime[2] = ':';
    strTime[3] = t->minute / 10 + '0';
    strTime[4] = t->minute % 10 + '0';
    strTime[5] = '\0';

    strDate[0] = t->month / 10 + '0';
    strDate[1] = t->month % 10 + '0';
    strDate[2] = '-';
    strDate[3] = t->day / 10 + '0';
    strDate[4] = t->day % 10 + '0';
    strDate[5] = '\0';


    setDrawColor(COLOR_0);
    drawBox(x, y, DISPLAY_WIDTH, DISPLAY_HEIGHT, FILLED);
    setDrawColor(COLOR_1);
    setFontColor(COLOR_1);
    
    setFontSize(4);
    setFontWeight(1);
    setFontSpacing(2);
    drawStr(x+62, y+6, strDate);
    if(t->week>=0 &&t->week<7)
        drawStr(x+17, y+6, STR_WEEK[t->week]);
    else
        drawStr(x+17, y+6, "ERR");
    setFontSize(9);
    setFontWeight(3);
    setFontSpacing(3);
    drawStr(x+1, y+30, strTime);
    drawBox(x+5, y, 5, 21, UNFILLED);
    drawBox(x+5, y, 5, 21*(t->second)/60 , FILLED);

    setDrawColor(COLOR_XOR);
    drawBox(x+11, y, 41, 21, FILLED);

    setDrawColor(COLOR_1);
    drawBox(x+11, y, 112, 21, UNFILLED);
}

void drawHumiture(int8 x,int8 y,humiture *dat){
    char strTemp[7], strHumi[6];
    sprintf(strTemp, "%.1f`", dat->temp);
    sprintf(strHumi, "%.1f%%", dat->humi);

    setDrawColor(COLOR_0);
    drawBox(x, y, DISPLAY_WIDTH, DISPLAY_HEIGHT, FILLED);
    setDrawColor(COLOR_1);
    setFontColor(COLOR_1);
    setFontSize(5);
    setFontWeight(2);
    setFontSpacing(2);
    drawStr(x+1, y+1, "TEMP");
    drawStr(x+1, y+19, "HUMI");
    setFontWeight(1);
    drawStr(x+62, y+1, strTemp);
    drawStr(x+62, y+19, strHumi);
}


void drawList(int8 x,int8 y, GUIListData *dat){
    int8 p;
    setDrawColor(COLOR_0);
    drawBox(x, y, DISPLAY_WIDTH, DISPLAY_HEIGHT, FILLED);
    setDrawColor(COLOR_1);
    setFontColor(COLOR_1);
    setFontSize(3);
    setFontSpacing(2);
    setFontWeight(1);
    drawStr90Deg(x+0, y+DISPLAY_HEIGHT - (DISPLAY_HEIGHT - getStrWidth(dat->title)) / 2, dat->title);
    drawVLine(x+12, y+0, DISPLAY_HEIGHT);

    setFontSize(4);
    p = (DISPLAY_HEIGHT - 9) / 2;
    drawStr(x + 22, y + p, dat->item[*dat->selection]);
     if(*(dat->selection)-1>=0) drawStr(x + 22, y + p-17, dat->item[*(dat->selection)-1]);
     if(*(dat->selection)+1<dat->itemNum) drawStr(x + 22, y + p+17, dat->item[*(dat->selection)+1]);

    // p = (DISPLAY_HEIGHT - 9) / 2;
    // //drawStr(18,p,(dat->item)[0]);
    // for (i = *(dat->selection); i < (dat->itemNum);++i){
    //     drawStr(x+22,y+p,(dat->item)[i]);
    //     p += 17;
    //     if(p>DISPLAY_HEIGHT)
    //         break;
    // }
    // p = (DISPLAY_HEIGHT - 9) / 2 - 17;
    // for (i = *(dat->selection) - 1; i >=0; --i){
    //     drawStr(x+22,y+p,(dat->item)[i]);
    //     p -= 17;
    //     if(p+9<0)
    //         break;
    // }

    setDrawColor(COLOR_XOR);
    drawBox(x+18,y+(DISPLAY_HEIGHT - 9) / 2-4, getStrWidth((dat->item)[*(dat->selection)]) + 8, 17,FILLED);
}


uint8 getNumStrLen(int16 num){
    if (num >= 0)
        return getNumDigital(num);
    else
        return getNumDigital(num) + 1;
}

void drawNumberSelect(int8 x,int8 y, GUINumberSelectData *dat){
    uint8 tmp1, tmp2, numWidth;
    char str[7];
    tmp1 = getNumStrLen(*(dat->min));
    tmp2 = getNumStrLen(*(dat->max));
    sprintf(str, "%d", *(dat->val));

    setDrawColor(COLOR_0);
    drawBox(x, y, DISPLAY_WIDTH, DISPLAY_HEIGHT, FILLED);
    setDrawColor(COLOR_1);
    setFontColor(COLOR_1);
    setFontWeight(1);
    setFontSize(4);
    setFontSpacing(2);
    drawStr(x+(DISPLAY_WIDTH - getStrWidth(dat->title)) / 2, y, dat->title);

    setFontSize(7);
    setFontWeight(2);
    numWidth = getStrWidth(str);
    drawBox(x+(DISPLAY_WIDTH-(getStrWidthByNum((tmp1>tmp2)?tmp1:tmp2)+16))/2,y+17,getStrWidthByNum((tmp1>tmp2)?tmp1:tmp2)+16,37,FILLED);
    setFontColor(COLOR_0);
    drawStr(x+(DISPLAY_WIDTH-(numWidth))/2,y+28,str);
    // for (i = 0; *(dat->val) + i * *(dat->step) <= *(dat->max);i++){
    //     if(p>(DISPLAY_HEIGHT-37)/2+37)
    //         break;
    //     drawNum(x + 16, y + p, *(dat->val) + i * *(dat->step));
    //     p += 19;
    // }
}

void drawCountdown(int8 x,int8 y,GUICountdownData* dat){
    char strTime[6];
    uint8 minute = dat->left / 60, second = dat->left % 60;
    strTime[0] = minute / 10 + '0';
    strTime[1] = minute % 10 + '0';
    strTime[2] = ':';
    strTime[3] = second / 10 + '0';
    strTime[4] = second % 10 + '0';
    strTime[5] = '\0';

    setDrawColor(COLOR_0);
    drawBox(x, y, DISPLAY_WIDTH, DISPLAY_HEIGHT, FILLED);
    setDrawColor(COLOR_1);
    setFontColor(COLOR_1);
    setFontSize(8);
    setFontWeight(2);
    setFontSpacing(3);
    if(dat->running)
        drawStr(x+12, y+22, strTime);
    else if(dat->left)
        drawStr(x+12, y+22, "PAUSE");
    else
        drawStr(x+20, y+22, "OVER");
    setDrawColor(COLOR_XOR);
    drawBox(x, y + 10, (uint32)DISPLAY_WIDTH*dat->left/dat->total, 41, FILLED);
    setDrawColor(COLOR_1);
    drawBox(x, y + 10, DISPLAY_WIDTH, 41, UNFILLED);
}

void drawAlarm(int8 x,int8 y,alarmData *dat){
    char str[6];
    str[0] = dat->hour / 10 + '0';
    str[1] = dat->hour % 10 + '0';
    str[2] = ':';
    str[3] = dat->minute / 10 + '0';
    str[4] = dat->minute % 10 + '0';
    str[5] = '\0';

    setDrawColor(COLOR_0);
    drawBox(x, y, DISPLAY_WIDTH, DISPLAY_HEIGHT, FILLED);
    setDrawColor(COLOR_1);
    setFontColor(COLOR_1);
    setFontSize(9);
    setFontWeight(3);
    setFontSpacing(3);
    drawStr(x+1, y+20, str);
    setDrawColor(COLOR_XOR);
    drawBox(31, 0, 64, 64, FILLED);
}

void drawText(int8 x,int8 y,char*text){
    uint8 width;
    setDrawColor(COLOR_0);
    drawBox(x, y, DISPLAY_WIDTH, DISPLAY_HEIGHT, FILLED);
    setDrawColor(COLOR_1);
    setFontColor(COLOR_1);
    setFontWeight(1);
    setFontSpacing(2);
    setFontSize(4);
    width = getStrWidth(text);
    drawStr((DISPLAY_WIDTH - width) / 2, 27, text);
}