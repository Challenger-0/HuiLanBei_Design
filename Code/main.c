#include "STC15W.h"
#include "INTRINS.H"
#include "GUI.h"
#include "PCF8563T.h"
#include "Timer.h"
#include "graphics.h"
#include "MATH.H"
#include "note.h"
#include "I2C.h"
#include "UART.h"
#include "AHT20.h"
#include "WS2812.h"
#include "MP3Player.h"
#include "E2PROM .h"

#define VERSION_INFO "VER:1.0\n"

#define PIN_KEY_DOWN P36
#define PIN_KEY_FUNC P33
#define PIN_KEY_UP   P32

#define KEY_DOWN 0
#define KEY_FUNC 1
#define KEY_UP   2

#define KEY_LONG_PRESS_TIME 300 //标志按下多久才算长按，单位毫秒
#define STANDBY_TIME 10000      //标志闲置多久才进入待机模式，单位毫秒

uint32 xdata millis = 0;

time xdata nowTime;
humiture xdata nowHumiture = {0.0,0.0};

GUICountdownData xdata countdownData = {60, 60,false};

uint8 xdata easterEggCount = 0;//按照常理，关于界面里总会有个彩蛋...

alarmData xdata alarmDat[5] = {{DISABLE, 8, 0}, {DISABLE, 8, 30}, {DISABLE, 9, 0}, {DISABLE, 9, 30}, {DISABLE, 10, 0}};
uint8 nowEditAlarm,nowRingingAlarm;

uint32 standbyTimeCount =0 ;
uint8 isStandbyMode = false;

uint8 getTimeFlag = false, setTimeFlag = false;

uint8 code alarmRingtone[] = {NOTESEQ_QIFENGLE, NOTESEQ_WEN, NOTESEQ_BEEP};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                                     LED

color xdata LEDActData[] = {{141, 241, 255}, {157, 223, 255}, {175, 158, 255}, {187, 174, 255}, {202, 137, 255}, {213, 91, 255}};

const color code colorBasic[][LED_NUM] = {//纯色
    {{255,255,255},{255,255,255},{255,255,255},{255,255,255},{255,255,255},{255,255,255}},//white
    {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}//white
};


const color code colorGradual[][LED_NUM] = {//渐变
    {{100,138,251},{124,134,248},{142,124,245},{180,109,241},{205,83,236},{229,20,232}}, //midnight
    {{121,255,130},{138,212,144},{160,190,167},{180,160,190},{197,120,211},{213,28,233}}, //earlyMorning
    {{169,8,234},{178,23,228},{191,32,219},{205,33,209},{217,27,200},{228,13,192}},       //morning
    {{232,24,190},{232,101,194},{231,150,199},{229,186,204},{226,221,210},{224,224,214}}, //afternoon
    {{138,255,200},{152,229,203},{171,235,206},{188,240,209},{204,245,212},{218,249,214}},//dusk
    {{76,83,251},{85,117,243},{95,148,233},{108,177,222},{119,198,212},{129,216,203}}     //night
};

uint8 xdata LEDBrightnessMask = 128;//亮度蒙版

uint8 LEDColor;

#define LED_EFFECT_MASK_DEFAULT  0
#define LED_EFFECT_MASK_KEY_DOWN 1
#define LED_EFFECT_MASK_KEY_FUNC 2
#define LED_EFFECT_MASK_KEY_UP   3

const uint8 code LEDEffectMask[][LED_NUM] = {//效果蒙版
    {31,31,31,31,31,31},//default
    {255,255,31,31,31,31},//k1 press
    {31,31,255,255,31,31},//k2 press
    {31,31,31,31,255,255} //k3 press
};

#define LED_KEY_PRESS_EFFECT_TIME 1500//表示按键按下光效持续多久，单位毫秒
uint8 xdata lastKeyPress, LEDKeyPressEffectPlaying = false;
uint32 LEDKeyPressEffectStartTime;

#define LED_UPDATE_TIME_MIN 25 //压制帧率，标志最快多少毫秒刷新一次LED
uint32 xdata LEDLastUpdateTime = 0;//上次刷新屏幕的时间

uint8 LEDOn = true;
uint8 LEDLightMode=false;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                                     EEPROM数据
uint8 EEPROMUpdate = false;//当此位为true时，会择机与eeprom同步数据（注意不是立即同步）
//uint8 xdata settingsData[128];//此空间用于集中存储设置数据，然后一次性与eeprom同步
// time xdata *code nowtime = settingsData + 0;
// alarmData xdata *code alarmDat = settingsData + 0 + 7;
// uint8


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                                     声音

#define SOUND_TYPE_MUSIC 0
#define SOUND_TYPE_8_BIT 1
uint8 soundType=SOUND_TYPE_MUSIC;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                                     UI数据

char code *listItemFunc[] = {"LIGHT","ALARM","COUNTDOWN","SETTINGS"};
uint8  xdata listFuncSelection=0;
GUIListData  code listFuncData = {"FUNC",&listItemFunc,4,&listFuncSelection};

char code *listItemAlarmList[] = {"ALARM1","ALARM2","ALARM3","ALARM4","ALARM5"};
uint8  xdata listAlarmListSelection=0;
GUIListData  code listAlarmListData = {"ALARM",&listItemAlarmList,5,&listAlarmListSelection};

char code *listItemSettings[] = {"TIME|DATE","DISPLAY","SOUND","ABOUT","RESET"};
uint8  xdata listSettingsSelection=0;
GUIListData  code listSettingsData = {"SET",&listItemSettings,5,&listSettingsSelection};

char code *listItemCountdownControl[] = {"START","Minute","Second"};
uint8  xdata listCountdownControlSelection=0;
GUIListData  code listCountdownControlData = {"CNTDN",&listItemCountdownControl,3,&listCountdownControlSelection};

char code *listItemTimeSettings[] = {"YEAR","MONTH","DAY","HOUR","MINUTE"};
uint8  xdata listTimeSettingsSelection=0;
GUIListData  code listTimeSettingsData = {"TIME",&listItemTimeSettings,5,&listTimeSettingsSelection};

char code *listItemAbout[] = {"VER:","0.1","PG:","CHALLENGER","UI:","CHALLENGER","GITHUB:","CHA...ER-0"};
uint8  xdata listAboutSelection=0;
GUIListData  code listAboutData = {"ABOUT",&listItemAbout,8,&listAboutSelection};

char code *listItemAlarmSettings[] = {"ON/OFF","Hour","Minute","RINGTONE"};
uint8  xdata listAlarmSettingsSelection=0;
GUIListData  code listAlarmSettingsData = {"ALARM",&listItemAlarmSettings,4,&listAlarmSettingsSelection};

char code *listItemAlarmOnOff[] = {"OFF","ON"};
uint8  xdata listAlarmOnOffSelection=0;
GUIListData  code listAlarmOnOffData = {"ON/OFF",&listItemAlarmOnOff,2,&listAlarmOnOffSelection};

char code *listItemReset[] = {"RESTART","UPDATE","ERASE"};
uint8  xdata listResetSelection=0;
GUIListData  code listResetData = {"RESET",&listItemReset,3,&listResetSelection};

char code *listItemEraseVerify[] = {"NO","YES"};
uint8  xdata listEraseVerifySelection=0;
GUIListData  code listEraseVerifyData = {"ERASE?",&listItemEraseVerify,2,&listEraseVerifySelection};

char code *listItemAlarmRingtone[] = {"qifengle", "wen", "Beep"};
uint8  xdata listAlarmRingtoneSelection=0;
GUIListData  code listAlarmRingtoneData = {"RING",&listItemAlarmRingtone,3,&listAlarmRingtoneSelection};


char code *listItemDiaplaySettings[] = {"contrast","led","auto lock"};
uint8  xdata listDiaplaySettingsSelection=0;
GUIListData  code listDiaplaySettingsData = {"DISPLAY",&listItemDiaplaySettings,3,&listDiaplaySettingsSelection};

char code *listItemLEDSettings[] = {"switch","brightness","color"};
uint8  xdata listLEDSettingsSelection=0;
GUIListData  code listLEDSettingsData = {"LED",&listItemLEDSettings,3,&listLEDSettingsSelection};

char code *listItemLEDOnOff[] = {"off","on"};
uint8  xdata listLEDOnOffSelection=0;
GUIListData  code listLEDOnOffData = {"SWITCH",&listItemLEDOnOff,2,&listLEDOnOffSelection};

char code *listItemLEDColor[] = {"WHITH","SKY"};
uint8  xdata listLEDColorSelection=0;
GUIListData  code listLEDColorData = {"COLOR",&listItemLEDColor,2,&listLEDColorSelection};

char code *listItemSoundSettings[] = {"TYPE","VOLUME"};
uint8  xdata listSoundSettingsSelection=0;
GUIListData  code listSoundSettingsData = {"SOUND",&listItemSoundSettings,2,&listSoundSettingsSelection};

char code *listItemSoundType[] = {"normal","8-bit"};
uint8  xdata listSoundTypeSelection=0;
GUIListData  code listSoundTypeData = {"SOUND",&listItemSoundType,2,&listSoundTypeSelection};



//====================================================================================================

int16 xdata numberSelectYearMax = 2099, numberSelectYearMin = 2000, numberSelectYearStep = 1, numberSelectYearValue;
GUINumberSelectData code numberSelectYearData = {"YEAR", &numberSelectYearMax, &numberSelectYearMin,&numberSelectYearValue, &numberSelectYearStep};

int16 xdata numberSelectMonthMax = 12, numberSelectMonthMin = 1, numberSelectMonthStep = 1, numberSelectMonthValue;
GUINumberSelectData code numberSelectMonthData = {"MONTH", &numberSelectMonthMax, &numberSelectMonthMin,&numberSelectMonthValue, &numberSelectMonthStep};

int16 xdata numberSelectDayMax , numberSelectDayMin = 1, numberSelectDayStep = 1, numberSelectDayValue;
GUINumberSelectData code numberSelectDayData = {"Day", &numberSelectDayMax, &numberSelectDayMin,&numberSelectDayValue, &numberSelectDayStep};

int16 xdata numberSelectHourMax = 23, numberSelectHourMin = 0, numberSelectHourStep = 1, numberSelectHourValue;
GUINumberSelectData code numberSelectHourData = {"Hour", &numberSelectHourMax, &numberSelectHourMin,&numberSelectHourValue, &numberSelectHourStep};


int16 xdata numberSelectMinuteMax = 59, numberSelectMinuteMin = 0, numberSelectMinuteStep = 1, numberSelectMinuteValue;
GUINumberSelectData code numberSelectMinuteData = {"Minute", &numberSelectMinuteMax, &numberSelectMinuteMin,&numberSelectMinuteValue, &numberSelectMinuteStep};

int16 xdata numberSelectCDMinuteMax = 99, numberSelectCDMinuteMin = 0, numberSelectCDMinuteStep = 1, numberSelectCDMinuteValue;
GUINumberSelectData code numberSelectCDMinuteData = {"Minute", &numberSelectCDMinuteMax, &numberSelectCDMinuteMin,&numberSelectCDMinuteValue, &numberSelectCDMinuteStep};

int16 xdata numberSelectCDSecondMax = 59, numberSelectCDSecondMin = 0, numberSelectCDSecondStep = 1, numberSelectCDSecondValue;
GUINumberSelectData code numberSelectCDSecondData = {"Second", &numberSelectCDSecondMax, &numberSelectCDSecondMin,&numberSelectCDSecondValue, &numberSelectCDSecondStep};

int16 xdata numberSelectAlarmHourMax = 23, numberSelectAlarmHourMin = 0, numberSelectAlarmHourStep = 1, numberSelectAlarmHourValue;
GUINumberSelectData code numberSelectAlarmHourData = {"Hour", &numberSelectAlarmHourMax, &numberSelectAlarmHourMin,&numberSelectAlarmHourValue, &numberSelectAlarmHourStep};

int16 xdata numberSelectAlarmMinuteMax = 59, numberSelectAlarmMinuteMin = 0, numberSelectAlarmMinuteStep = 1, numberSelectAlarmMinuteValue;
GUINumberSelectData code numberSelectAlarmMinuteData = {"Minute", &numberSelectAlarmMinuteMax, &numberSelectAlarmMinuteMin,&numberSelectAlarmMinuteValue, &numberSelectAlarmMinuteStep};


int16 xdata numberSelectContrastMax = 255, numberSelectContrastMin = 0, numberSelectContrastStep = 1, numberSelectContrastValue;
GUINumberSelectData code numberSelectContrastData = {"Contrast", &numberSelectContrastMax, &numberSelectContrastMin,&numberSelectContrastValue, &numberSelectContrastStep};

int16 xdata numberSelectLEDBrightnessMax = 255, numberSelectLEDBrightnessMin = 0, numberSelectLEDBrightnessStep = 1, numberSelectLEDBrightnessValue;
GUINumberSelectData code numberSelectLEDBrightnessData = {"BRIGHTNESS", &numberSelectLEDBrightnessMax, &numberSelectLEDBrightnessMin,&numberSelectLEDBrightnessValue, &numberSelectLEDBrightnessStep};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                                     界面部分

#define INTERFACE_TYPE_MAIN          0
#define INTERFACE_TYPE_HUMITURE      1
#define INTERFACE_TYPE_LIST          2
#define INTERFACE_TYPE_NUMBER_SELECT 3
#define INTERFACE_TYPE_COUNTDOWN     4
#define INTERFACE_TYPE_ALARM         5

typedef struct{
    uint8 type;
    void *dat;
}interfaceDat;

#define INTERFACE_MAIN       0
#define INTERFACE_FUNC       1
#define INTERFACE_HUMITURE   2
#define INTERFACE_ALARM_LIST 3
#define INTERFACE_SETTINGS   4
#define INTERFACE_COUNTDOWN_CONTROL   5
#define INTERFACE_COUNTDOWN  6
#define INTERFACE_TIME_SETTINGS 7
#define INTERFACE_YEAR_SETTINGS 8
#define INTERFACE_ABOUT      9
#define INTERFACE_MONTH_SETTINGS 10
#define INTERFACE_DAY_SETTINGS  11
#define INTERFACE_HOUR_SETTINGS 12
#define INTERFACE_MINUTE_SETTIINGS 13
#define INTERFACE_ALARM_SETTINGS 14
#define INTERFACE_CD_MINUTE_SETTINGS 15
#define INTERFACE_CD_SECOND_SETTINGS 16
#define INTERFACE_ALARM_ON_OFF 17 
#define INTERFACE_ALARM_HOUR_SETTINGS 18
#define INTERFACE_ALARM_MINUTE_SETTINGS 19
#define INTERFACE_ALARM       20
#define INTERFACE_ALARM_RINGTONE_SETTINGS 21
#define INTERFACE_DISPLAY_SETTINGS 22
#define INTERFACE_CONTRAST_SETTINGS 23
#define INTERFACE_LED_SETTINGS 24
#define INTERFACE_LED_COLOR  25
#define INTERFACE_LED_BRIGHTNESS 26
#define INTERFACE_LED_ON_OFF     27

const interfaceDat code interfaceData[] = {{INTERFACE_TYPE_MAIN,(void*)&nowTime},
                                           {INTERFACE_TYPE_LIST,(void*)&listFuncData},
                                           {INTERFACE_TYPE_HUMITURE,(void*)&nowHumiture},
                                           {INTERFACE_TYPE_LIST,(void*)&listAlarmListData},
                                           {INTERFACE_TYPE_LIST,(void*)&listSettingsData},
                                           {INTERFACE_TYPE_LIST,(void*)&listCountdownControlData},
                                           {INTERFACE_TYPE_COUNTDOWN,(void*)&countdownData},
                                           {INTERFACE_TYPE_LIST,(void*)&listTimeSettingsData},
                                           {INTERFACE_TYPE_NUMBER_SELECT,(void*)&numberSelectYearData},
                                           {INTERFACE_TYPE_LIST,(void*)&listAboutData},
                                           {INTERFACE_TYPE_NUMBER_SELECT,(void*)&numberSelectMonthData},
                                           {INTERFACE_TYPE_NUMBER_SELECT,(void*)&numberSelectDayData},
                                           {INTERFACE_TYPE_NUMBER_SELECT,(void*)&numberSelectHourData},
                                           {INTERFACE_TYPE_NUMBER_SELECT,(void*)&numberSelectMinuteData},
                                           {INTERFACE_TYPE_LIST,(void*)&listAlarmSettingsData},
                                           {INTERFACE_TYPE_NUMBER_SELECT,(void*)&numberSelectCDMinuteData},
                                           {INTERFACE_TYPE_NUMBER_SELECT,(void*)&numberSelectCDSecondData},
                                           {INTERFACE_TYPE_LIST,(void*)&listAlarmOnOffData},
                                           {INTERFACE_TYPE_NUMBER_SELECT,(void*)&numberSelectAlarmHourData},
                                           {INTERFACE_TYPE_NUMBER_SELECT,(void*)&numberSelectAlarmMinuteData},
                                           {INTERFACE_TYPE_ALARM,(void*)alarmDat},
                                           {INTERFACE_TYPE_LIST,(void*)&listAlarmRingtoneData},
                                           {INTERFACE_TYPE_LIST,(void*)&listDiaplaySettingsData},
                                           {INTERFACE_TYPE_NUMBER_SELECT,(void*)&numberSelectContrastData},
                                           {INTERFACE_TYPE_LIST,(void*)&listLEDSettingsData},
                                           {INTERFACE_TYPE_LIST,(void*)&listLEDColorData},
                                           {INTERFACE_TYPE_NUMBER_SELECT,(void*)&numberSelectLEDBrightnessData},
                                           {INTERFACE_TYPE_LIST,(void*)&listLEDOnOffData}
                                        };

uint8 xdata interfaceSequence[8] = {INTERFACE_MAIN};//此数组用于记录各界面的层级关系。changeScreen同层级切换，nextScreen增加上层，prevScreen返回下层
uint8 xdata interfaceSequenceIndex=0;
uint8 xdata interfacePrev;//上一个界面


//压帧率用
#define INTERFACE_UPDATE_TIME_MIN 50 //压制帧率，标志最快多少毫秒刷新一次屏幕
uint8 xdata interfaceUpdate = true;//当该位置1时刷新屏幕，避免长时间占用I2C总线
uint8 xdata interfaceForceUpdate = true;//忽略所有压帧率规则强制刷新一次
uint32 xdata interfaceLastUpdateTime = 0;//上次刷新屏幕的时间


//界面转场动画类型
#define INTERFACE_ANI_IN          0 //进入新界面
#define INTERFACE_ANI_OUT         1 //返回上一层
#define INTERFACE_ANI_LEFT_SLIDE  2 //同层切换向左
#define INTERFACE_ANI_RIGHT_SLIDE 3 //同层切换向右

uint8 xdata interfaceAniType;
uint8 xdata interfaceAniPlaying = false;//标志是否在播放动画

#define INTERFACE_ANI_TIME 300 //动画时长ms
uint32 xdata interfaceAniStartTime;//动画开始的时间

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                                   声明

void keyClickHandle(uint8 key);
void keyLongPressHandle(uint8 key);
void keyHandle();
void drawInterface(int8 x, int8 y, interfaceDat *dat);
void graphics();
uint8 isLeapYear(uint8 year);
uint8 getMonthDay(uint8 year, uint8 month);
void localTimeUpdate();
void playInterfaceAnimation(uint8 type);
void nextInterface(uint8 index);
void prevInterface();
void changeInterface(uint8 index);
uint8 getMonthDay(uint8 year, uint8 month);
void weekdayUpdate(time *dat);
void playSound(uint16 track);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                                   正文





//===================================================================================================================
//===================================================短按处理==========================================================
//===================================================================================================================

void keyClickHandle(uint8 key){
    void *dat = interfaceData[interfaceSequence[interfaceSequenceIndex]].dat;
    interfaceUpdate = true;
    switch(key){
        case KEY_DOWN:
            switch(interfaceData[interfaceSequence[interfaceSequenceIndex]].type){
                case INTERFACE_TYPE_MAIN:
                    changeInterface(INTERFACE_HUMITURE);
                    break;
                case INTERFACE_TYPE_LIST:
                    if(*((GUIListData*)dat)->selection!=0)
                        --*((GUIListData *)dat)->selection;
                    else
                        playSound(NOTRSEQ_WARNINGS);
                    break;
                case INTERFACE_TYPE_HUMITURE:
                    changeInterface(INTERFACE_MAIN);
                    break;
                case INTERFACE_TYPE_NUMBER_SELECT:
                    if(*((GUINumberSelectData*)dat)->val==*((GUINumberSelectData*)dat)->min){
                        playSound(NOTRSEQ_WARNINGS);
                        break;
                    }
                    if(*((GUINumberSelectData*)dat)->val-*((GUINumberSelectData*)dat)->step>=*((GUINumberSelectData*)dat)->min)
                        *((GUINumberSelectData *)dat)->val -= *((GUINumberSelectData *)dat)->step;
                    else
                        *((GUINumberSelectData *)dat)->val = *((GUINumberSelectData *)dat)->min;
                    break;
                case INTERFACE_TYPE_COUNTDOWN:
                    if(countdownData.left>=60){
                        countdownData.left-=60;
                        countdownData.total-=60;
                    }
                    break;
            }
            break;
//-----------------------------------------------------------------------------------
        case KEY_FUNC:
            switch(interfaceSequence[interfaceSequenceIndex]){
                case INTERFACE_MAIN:
                    nextInterface(INTERFACE_FUNC);
                    break;
                case INTERFACE_FUNC:
                    switch(listFuncSelection){
                        case 0:
                            LEDLightMode = !LEDLightMode;
                            break;
                        case 1:
                            nextInterface(INTERFACE_ALARM_LIST);
                            break;
                        case 2:
                            nextInterface(INTERFACE_COUNTDOWN_CONTROL);
                            break;
                        case 3:
                            nextInterface(INTERFACE_SETTINGS);
                            break;
                    }
                    break;
                case INTERFACE_ALARM_LIST:
                    nowEditAlarm = listAlarmListSelection;
                    nextInterface(INTERFACE_ALARM_SETTINGS);
                    break;
                case INTERFACE_COUNTDOWN_CONTROL:
                    switch(listCountdownControlSelection){
                        case 0:
                            nextInterface(INTERFACE_COUNTDOWN);
                            if (!countdownData.left&&!countdownData.running){
                                countdownData.total = numberSelectCDMinuteValue * 60 + numberSelectCDSecondValue;
                                countdownData.left = countdownData.total;
                                countdownData.running = true;
                            }
                            break;
                        case 1:
                            nextInterface(INTERFACE_CD_MINUTE_SETTINGS);
                            break;
                        case 2:
                            nextInterface(INTERFACE_CD_SECOND_SETTINGS);
                            break;
                    }
                    break;
                case INTERFACE_SETTINGS:
                    switch(listSettingsSelection){
                        case 0:
                            nextInterface(INTERFACE_TIME_SETTINGS);
                            break;
                        case 1:
                            nextInterface(INTERFACE_DISPLAY_SETTINGS);
                            break;
                        case 3:
                            nextInterface(INTERFACE_ABOUT);
                            break;
                        case 4:
                            IAP_CONTR = 0x60;//软复位到ISP区
                            break;
                    } 
                    break;
                case INTERFACE_TIME_SETTINGS:
                    switch(listTimeSettingsSelection){
                        case 0:
                            nextInterface(INTERFACE_YEAR_SETTINGS);
                            numberSelectYearValue = nowTime.year + 2000;
                            break;
                        case 1:
                            nextInterface(INTERFACE_MONTH_SETTINGS);
                            numberSelectMonthValue = nowTime.month;
                            break;
                        case 2:
                            nextInterface(INTERFACE_DAY_SETTINGS);
                            numberSelectDayMax = getMonthDay(nowTime.year,nowTime.month);
                            numberSelectDayValue = nowTime.day;
                            break;
                        case 3:
                            nextInterface(INTERFACE_HOUR_SETTINGS);
                            numberSelectMinuteValue=nowTime.hour;
                            break;
                        case 4:
                            nextInterface(INTERFACE_MINUTE_SETTIINGS);
                            numberSelectMinuteValue = nowTime.minute;
                            break;
                    }
                    break;
                case INTERFACE_ALARM_SETTINGS:
                    switch(listAlarmSettingsSelection){
                        case 0:
                            nextInterface(INTERFACE_ALARM_ON_OFF);
                            listAlarmOnOffSelection = alarmDat[nowEditAlarm].enable;
                            break;
                        case 1:
                            nextInterface(INTERFACE_ALARM_HOUR_SETTINGS);
                            numberSelectAlarmHourValue = alarmDat[nowEditAlarm].hour;
                            break;
                        case 2:
                            nextInterface(INTERFACE_ALARM_MINUTE_SETTINGS);
                            numberSelectAlarmMinuteValue = alarmDat[nowEditAlarm].minute;
                            break;
                        case 3:
                            nextInterface(INTERFACE_ALARM_RINGTONE_SETTINGS);
                            listAlarmRingtoneSelection = alarmDat[nowEditAlarm].ringTone;
                            break;
                    }
                    break;
                case INTERFACE_ALARM_ON_OFF:
                    alarmDat[nowEditAlarm].enable = listAlarmOnOffSelection;
                    EEPROMUpdate = true;
                    playSound(NOTESEQ_SUCCESS);
                    prevInterface();
                    break;
                case INTERFACE_ALARM_HOUR_SETTINGS:
                    alarmDat[nowEditAlarm].hour = numberSelectAlarmHourValue;
                    EEPROMUpdate = true;
                    playSound(NOTESEQ_SUCCESS);
                    prevInterface();
                    break;
                case INTERFACE_ALARM_MINUTE_SETTINGS:
                    alarmDat[nowEditAlarm].minute = numberSelectAlarmMinuteValue;
                    EEPROMUpdate = true;
                    playSound(NOTESEQ_SUCCESS);
                    prevInterface();
                    break;
                case INTERFACE_COUNTDOWN:
                    if(countdownData.running)
                        countdownData.running = !countdownData.running;
                    else if (countdownData.left)
                        countdownData.running = !countdownData.running;
                    else
                        prevInterface();
                    break;
                case INTERFACE_YEAR_SETTINGS:
                    nowTime.year = numberSelectYearValue - 2000;
                    setTimeFlag = true;
                    weekdayUpdate(&nowTime);
                    prevInterface();
                    playSound(NOTESEQ_SUCCESS);
                    break;
                case INTERFACE_MONTH_SETTINGS:
                    nowTime.month = numberSelectMonthValue;
                    setTimeFlag = true;
                    weekdayUpdate(&nowTime);
                    if(nowTime.day>getMonthDay(nowTime.year,nowTime.month))
                        nowTime.day = getMonthDay(nowTime.year,nowTime.month);
                    prevInterface();
                    playSound(NOTESEQ_SUCCESS);
                    break;
                case INTERFACE_DAY_SETTINGS:
                    nowTime.day = numberSelectDayValue;
                    setTimeFlag = true;
                    weekdayUpdate(&nowTime);
                    prevInterface();
                    playSound(NOTESEQ_SUCCESS);
                    break;
                case INTERFACE_HOUR_SETTINGS:
                    nowTime.hour = numberSelectHourValue;
                    setTimeFlag = true;
                    prevInterface();
                    playSound(NOTESEQ_SUCCESS);
                    break;
                case INTERFACE_MINUTE_SETTIINGS:
                    nowTime.minute = numberSelectMinuteValue;
                    setTimeFlag = true;                                                                                                                              setTime(&nowTime);
                    prevInterface();
                    playSound(NOTESEQ_SUCCESS);
                    break;
                case INTERFACE_CD_MINUTE_SETTINGS:
                    prevInterface();
                    playSound(NOTESEQ_SUCCESS);
                    break;
                case INTERFACE_CD_SECOND_SETTINGS:
                    prevInterface();
                    playSound(NOTESEQ_SUCCESS);
                    break;
                case INTERFACE_ABOUT:
                    easterEggCount++;
                    if(easterEggCount==10){
                        easterEggCount = 0;
                        playSound(NOTESEQ_QIFENGLE);
                    }
                    break;
                case INTERFACE_ALARM_RINGTONE_SETTINGS:
                    alarmDat[nowEditAlarm].ringTone = listAlarmRingtoneSelection;
                    EEPROMUpdate = true;
                    prevInterface();
                    playSound(NOTESEQ_SUCCESS);
                    break;
                case INTERFACE_DISPLAY_SETTINGS:
                    switch(listDiaplaySettingsSelection){
                        case 0:
                             nextInterface(INTERFACE_CONTRAST_SETTINGS);
                             break;
                        case 1:
                            nextInterface(INTERFACE_LED_SETTINGS);
                           break;
                    }
                    break;
                case INTERFACE_LED_SETTINGS:
                    switch(listLEDSettingsSelection){
                        case 0:
                            nextInterface(INTERFACE_LED_ON_OFF);
                            break;
                        case 2:
                            nextInterface(INTERFACE_LED_COLOR);
                            break;
                        case 1:
                            nextInterface(INTERFACE_LED_BRIGHTNESS);
                            numberSelectLEDBrightnessValue = LEDBrightnessMask;
                            break;
                    }
                break;
                case INTERFACE_CONTRAST_SETTINGS:
                    setContrast(numberSelectContrastValue);
                    prevInterface();

                    playSound(NOTESEQ_SUCCESS);
                    break;
                case INTERFACE_LED_ON_OFF:
                    LEDOn=listLEDOnOffSelection;
                    playSound(NOTESEQ_SUCCESS);
                    WS2812Update(colorBasic[1]);
                    prevInterface();
                    break;
                case INTERFACE_LED_BRIGHTNESS:
                    LEDBrightnessMask = numberSelectLEDBrightnessValue;
                    playSound(NOTESEQ_SUCCESS);
                    prevInterface();
                    break;
                case INTERFACE_LED_COLOR:
                    LEDColor = listLEDColorSelection;
                    prevInterface();
                    break;
            }
            break;
//---------------------------------------------------------------------------------------------
        case KEY_UP:
            switch(interfaceData[interfaceSequence[interfaceSequenceIndex]].type){
                case INTERFACE_TYPE_MAIN:
                    changeInterface(INTERFACE_HUMITURE);
                    break;
                case INTERFACE_TYPE_LIST:
                    if(*((GUIListData*)interfaceData[interfaceSequence[interfaceSequenceIndex]].dat)->selection<((GUIListData*)interfaceData[interfaceSequence[interfaceSequenceIndex]].dat)->itemNum-1)
                        ++*((GUIListData *)interfaceData[interfaceSequence[interfaceSequenceIndex]].dat)->selection;
                    else
                        playSound(NOTRSEQ_WARNINGS);
                    break;
                case INTERFACE_TYPE_HUMITURE:
                    changeInterface(INTERFACE_MAIN);
                    break;
                case INTERFACE_TYPE_NUMBER_SELECT:
                    if(*((GUINumberSelectData*)dat)->val==*((GUINumberSelectData*)dat)->max){
                        playSound(NOTRSEQ_WARNINGS);
                        break;
                    }
                    if(*((GUINumberSelectData*)dat)->val+*((GUINumberSelectData*)dat)->step<=*((GUINumberSelectData*)dat)->max)
                        *((GUINumberSelectData *)dat)->val += *((GUINumberSelectData *)dat)->step;
                    else
                        *((GUINumberSelectData *)dat)->val = *((GUINumberSelectData *)dat)->max;
                    break;
                case INTERFACE_TYPE_COUNTDOWN:
                    if(countdownData.total<5940){
                        countdownData.left+=60;
                        countdownData.total += 60;
                    }
                    else if(countdownData.left<5940){
                        countdownData.left+=60;
                    }
                    break;
            }
            break;
    }
}

//===================================================================================================================
//===================================================长按处理==========================================================
//===================================================================================================================

void keyLongPressHandle(uint8 key){
    void *dat = interfaceData[interfaceSequence[interfaceSequenceIndex]].dat;
    interfaceUpdate = true;
    switch(key){
        case KEY_DOWN:
            switch(interfaceData[interfaceSequence[interfaceSequenceIndex]].type){
                case INTERFACE_TYPE_MAIN:
                    playSound(NOTRSEQ_WARNINGS);
                    break;
                case INTERFACE_TYPE_LIST:
                    break;
                case INTERFACE_TYPE_HUMITURE:
                    playSound(NOTRSEQ_WARNINGS);
                    break;
                case INTERFACE_TYPE_NUMBER_SELECT:
                    *((GUINumberSelectData *)dat)->val = *((GUINumberSelectData *)dat)->min;
                    playSound(NOTESEQ_DECREASE);
                    break;
                case INTERFACE_TYPE_COUNTDOWN:
                    countdownData.left = 0;
                    countdownData.running = false;
                    break;
            }
            break;
        case KEY_FUNC:
            prevInterface();
            break;
        case KEY_UP:
            switch(interfaceData[interfaceSequence[interfaceSequenceIndex]].type){
                case INTERFACE_TYPE_MAIN:
                    playSound(NOTRSEQ_WARNINGS);
                    break;
                case INTERFACE_TYPE_LIST:
                    break;
                case INTERFACE_TYPE_HUMITURE:
                    playSound(NOTRSEQ_WARNINGS);
                    break;
                case INTERFACE_TYPE_NUMBER_SELECT:
                    *((GUINumberSelectData *)dat)->val = *((GUINumberSelectData *)dat)->max;
                    playSound(NOTESEQ_INCREASE);
                    break;
                case INTERFACE_TYPE_COUNTDOWN:
                    countdownData.total = numberSelectCDMinuteValue * 60 + numberSelectCDSecondValue;
                    countdownData.left = countdownData.total;
                    countdownData.running = true;
                    break;
            }
            break;
    }
}

void keyHandle(){
    static uint8 xdata keyDownPress = false, keyFuncPress = false, keyUpPress = false,
                       keyDonwTriggered = false, keyFuncTriggered = false, keyUpTriggered = false;
    static uint32 xdata keyDownPressTime, keyFuncPressTime, keyUpPressTime;

    if(!PIN_KEY_DOWN&&!keyDownPress){//按键按下事件
        keyDownPress = true;
        keyDownPressTime=millis;
        standbyTimeCount = millis;
        isStandbyMode = false;
        lastKeyPress = KEY_DOWN;
        LEDKeyPressEffectPlaying = true;
        LEDKeyPressEffectStartTime = millis;
        playSound(NOTESEQ_DEFAULT);
    }
    else if(PIN_KEY_DOWN&&keyDownPress){//按键弹起事件
        keyDownPress = false;
        keyDonwTriggered = false;
        if (millis-keyDownPressTime<KEY_LONG_PRESS_TIME)//单击事件
            keyClickHandle(KEY_DOWN);
    }
    else if(millis-keyDownPressTime>=KEY_LONG_PRESS_TIME&&keyDownPress&&!keyDonwTriggered){//长按事件
        keyDonwTriggered = true;
        playSound(NOTESEQ_DEFAULT);
        keyLongPressHandle(KEY_DOWN);
    }
    //======================================================================================
    if(!PIN_KEY_FUNC&&!keyFuncPress){//按键按下事件
        keyFuncPress = true;
        keyFuncPressTime=millis;
        standbyTimeCount = millis;
        isStandbyMode = false;
        lastKeyPress = KEY_FUNC;
        LEDKeyPressEffectPlaying = true;
        LEDKeyPressEffectStartTime = millis;
        playSound(NOTESEQ_DEFAULT);
    }
    else if(PIN_KEY_FUNC&&keyFuncPress){//按键弹起事件
        keyFuncPress = false;
        keyFuncTriggered = false;
        if (millis-keyFuncPressTime<KEY_LONG_PRESS_TIME)//单击事件
            keyClickHandle(KEY_FUNC);
    }
    else if(millis-keyFuncPressTime>=KEY_LONG_PRESS_TIME&&keyFuncPress&&!keyFuncTriggered){//长按事件
        keyFuncTriggered = true;
        playSound(NOTESEQ_DEFAULT);
        keyLongPressHandle(KEY_FUNC);
    }
    //=========================================================================================
    if(!PIN_KEY_UP&&!keyUpPress){//按键按下事件
        keyUpPress = true;
        keyUpPressTime=millis;
        standbyTimeCount = millis;
        isStandbyMode = false;
        lastKeyPress = KEY_UP;
        LEDKeyPressEffectPlaying = true;
        LEDKeyPressEffectStartTime = millis;
        playSound(NOTESEQ_DEFAULT);
    }
    else if(PIN_KEY_UP&&keyUpPress){//按键弹起事件
        keyUpPress = false;
        keyUpTriggered = false;
        if (millis-keyUpPressTime<KEY_LONG_PRESS_TIME)//单击事件
            keyClickHandle(KEY_UP);
    }
    else if(millis-keyUpPressTime>=KEY_LONG_PRESS_TIME&&keyUpPress&&!keyUpTriggered){//长按事件
        keyUpTriggered = true;
        playSound(NOTESEQ_DEFAULT);
        keyLongPressHandle(KEY_UP);
    }
    
}

void playInterfaceAnimation(uint8 type){
    interfaceAniStartTime = millis;
    interfaceAniType = type;
    interfaceAniPlaying = true;
    interfaceUpdate = true;
}

void nextInterface(uint8 index){//进入下一层级
    if(interfaceSequenceIndex<8){
        interfacePrev = interfaceSequence[interfaceSequenceIndex];
        ++interfaceSequenceIndex;
        interfaceSequence[interfaceSequenceIndex] = index;
        interfaceUpdate = true;
        //playInterfaceAnimation(INTERFACE_ANI_IN);
    }
}




void prevInterface(){//返回上一层级ASDFGHJ;'

    if (interfaceSequenceIndex){
        interfacePrev = interfaceSequence[interfaceSequenceIndex];
        --interfaceSequenceIndex;
        interfaceUpdate = true;
        //playInterfaceAnimation(INTERFACE_ANI_OUT);
    }
}

void changeInterface(uint8 index){//同层级之间切换
    interfacePrev = interfaceSequence[interfaceSequenceIndex];
    interfaceSequence[interfaceSequenceIndex] = index;
    interfaceUpdate = true;
}

void drawInterface(int8 x, int8 y,interfaceDat *dat){
    switch(dat->type){
        case INTERFACE_TYPE_MAIN:
            drawMain(x, y, (time *)dat->dat);
            break;
        case INTERFACE_TYPE_HUMITURE:
            drawHumiture(x, y, (humiture *)dat->dat);
            break;
        case INTERFACE_TYPE_LIST:
            drawList(x, y, (GUIListData *)dat->dat);
            break;
        case INTERFACE_TYPE_NUMBER_SELECT:
            drawNumberSelect(x, y, (GUINumberSelectData *)dat->dat);
            break;
        case INTERFACE_TYPE_COUNTDOWN:
            drawCountdown(x, y, (GUICountdownData *)dat->dat);
            break;
        case INTERFACE_TYPE_ALARM:
            drawAlarm(x, y, &(((alarmData*)(dat->dat))[nowRingingAlarm]));
            break;
    }
}

void graphics(){
    // int8 x0, x1, y0, y1;
    // double p;
    if(interfaceUpdate && (millis-interfaceLastUpdateTime)>=INTERFACE_UPDATE_TIME_MIN || interfaceForceUpdate){//压帧率
        interfaceUpdate = false;
        interfaceForceUpdate = false;
        interfaceLastUpdateTime = millis;
        // if((millis-interfaceAniStartTime)>INTERFACE_ANI_TIME && interfaceAniPlaying){
        //     interfaceAniPlaying = false;
        //     interfaceUpdate = true;
        // }

        // if(interfaceAniPlaying){//绘制界面转场动画
        //     switch(interfaceAniType){
        //         interfaceUpdate = true;//保持刷新直到结束
        //         case INTERFACE_ANI_IN:
        //             p = pow((double)(millis - interfaceAniStartTime) / INTERFACE_ANI_TIME,3) + 1;
        //             x0 = x1 = 0;
        //             y1 = (DISPLAY_HEIGHT / 4) * p;
        //             y0 = DISPLAY_HEIGHT * p - DISPLAY_HEIGHT;
        //             drawInterface(x1, y1, &interfaceData[interfacePrev]);
        //             drawInterface(x0, y0, &interfaceData[interfaceSequence[interfaceSequenceIndex]]);
        //             drawBox(0, 0, 30, 30, FILLED);
        //             break;
        //         case INTERFACE_ANI_OUT:
        //             break;
        //         case INTERFACE_ANI_LEFT_SLIDE:
        //             break;
        //         case INTERFACE_ANI_RIGHT_SLIDE:
        //             break;
        //     }
        // }
        // else{
        //     drawInterface(0,0,&interfaceData[interfaceSequence[interfaceSequenceIndex]]);
        // }
        drawInterface(0, 0, &interfaceData[interfaceSequence[interfaceSequenceIndex]]);
        graphicsUpdate();
    }
}

void playSound(uint16 track){
    if(soundType==SOUND_TYPE_MUSIC){
        MP3PlayerPlayTrack(track);
    }
    else{
        playNoteSequence(track);
    }
}

uint8 isLeapYear(uint8 year){
    return !(year % 4);//在这个日期系统里只要能被4整除的都是闰年
}

uint8 getMonthDay(uint8 year, uint8 month){
    if(month==1||month==3||month==5||month==7||month==8||month==10||month==12)
        return 31;
    else if(month==4||month==6||month==9||month==11)
        return 30;
    else if (isLeapYear(year))
        return 29;
    else
        return 28;
}

void weekdayUpdate(time *dat){
    int16 m = dat->month, d = dat->day, c = 20, y = dat->year,year=dat->year+2000,w;
    // 根据月份对年份和月份进行调整
    if(m <= 2)
    {
        year -= 1;
        m += 12;
    } 
    // 根据泰勒公式计算星期
    w = (int)(c/4) - 2*c + y + (int)(y/4)+ (int)(13*(m+1)/5) + d - 1;

    dat->week = w % 7;
}

void countdownHandle(){//每秒被触发一次
    if(countdownData.running){
        countdownData.left--;
        if(!countdownData.left){
            countdownData.running = false;
            playSound(NOTESEQ_COUNTDOWN_OVER);
            if(interfaceSequence[interfaceSequenceIndex]!=INTERFACE_COUNTDOWN){
                nextInterface(INTERFACE_COUNTDOWN);
            }
        }
    }
}

void alarmhandle(){//每分钟被触发一次
    uint8 i;
    for (i = 0; i < 5;++i){

        if(alarmDat[i].minute==nowTime.minute){
            if(alarmDat[i].hour==nowTime.hour){
                nowRingingAlarm = i;
                nextInterface(INTERFACE_ALARM);
                playSound(alarmRingtone[alarmDat[i].ringTone]);
            }
        }
    }
}

void localTimeUpdate(){//只是刷新本地时间，不做同步
    static uint16 xdata ms = 0;
    ++ms;
    if (ms==1000){//虽然这样很不美观，但是只要知道是简单的时间进位就行了
        ms = 0;
        ++nowTime.second;
        countdownHandle();
        interfaceUpdate = true;
        if(nowTime.second==60){
            nowTime.second = 0;
            ++nowTime.minute;
            alarmhandle();
            interfaceUpdate = true;//分钟变化，刷新屏幕；
            if(nowTime.minute==60){
                nowTime.minute = 0;
                ++nowTime.hour;
                if(nowTime.hour==24){
                    nowTime.hour = 0;
                    ++nowTime.day;
                    if(nowTime.day>getMonthDay(nowTime.year,nowTime.month)){
                        nowTime.day = 1;
                        ++nowTime.month;
                        if(nowTime.month==13){
                            nowTime.month = 1;
                            ++nowTime.year;
                        }
                    }
                }
            }
        }
    }
}

void EEPROMHandle(){
    EEPROM_write_n(0, (uint8 *)alarmDat, 20);
}

void EEPROMRecover(){
    EEPROM_read_n(0, (uint8 *)alarmDat, 20);
}

void standbyHandle(){
    if(millis-standbyTimeCount>STANDBY_TIME&&!isStandbyMode){
        isStandbyMode = true;
        interfaceSequenceIndex = 0;//回到主界面
        if(interfaceSequence[interfaceSequenceIndex]!=INTERFACE_COUNTDOWN || interfaceSequence[interfaceSequenceIndex]!=INTERFACE_ALARM)
            interfaceSequence[interfaceSequenceIndex] = INTERFACE_MAIN;//除了闹钟响铃和倒计时界面，其他在待机时都自动回主界面
    }
    if(isStandbyMode){
        if(setTimeFlag){//这个时候就可以乱同步了，反正你也不操作
            setTimeFlag = false;
            setTime(&nowTime);
        }
        if(getTimeFlag){
            getTimeFlag = false;
            getTime(&nowTime);
        }
        if(EEPROMUpdate){
            EEPROMUpdate = false;
            EEPROMHandle();
        }
    }
}


void LEDHandle(){
    uint8 colorIndex = nowTime.hour / 4, i, j, *dat, byteNum = LED_NUM * 3, tmp1; ///;
    if(millis-LEDLastUpdateTime<LED_UPDATE_TIME_MIN||!LEDOn)
        return;
    LEDLastUpdateTime = millis;
    if(LEDColor){
        for (i = 0; i < LED_NUM;i++){
            LEDActData[i] = colorGradual[colorIndex][i];
        }
    }
    else{
        for (i = 0; i < LED_NUM;i++){
            LEDActData[i] = colorBasic[0][i];
        }
    }
    if(LEDLightMode){
        WS2812Update(LEDActData);
        return;
    }
    for (i = 0,dat=(uint8*)LEDActData; i < byteNum;i++,dat++){
        *dat = *dat * LEDBrightnessMask / 255;
    }
    if(LEDKeyPressEffectPlaying){
        if(millis-LEDKeyPressEffectStartTime>=LED_KEY_PRESS_EFFECT_TIME){
            LEDKeyPressEffectPlaying = false;
            return;
        }
        switch(lastKeyPress){
            case KEY_DOWN:
                for (i = 0,dat=(uint8*)LEDActData; i < LED_NUM;i++){
                    tmp1 = (LEDEffectMask[LED_EFFECT_MASK_KEY_DOWN][i] * (LEDKeyPressEffectStartTime + LED_KEY_PRESS_EFFECT_TIME - millis) / LED_KEY_PRESS_EFFECT_TIME + LEDEffectMask[LED_EFFECT_MASK_DEFAULT][i] * (millis - LEDKeyPressEffectStartTime) / LED_KEY_PRESS_EFFECT_TIME);
                    for (j = 0; j < 3;j++,dat++){
                        *dat = (*dat) * tmp1 / 255;
                    }
                }
            break;
            case KEY_FUNC:
                for (i = 0,dat=(uint8*)LEDActData; i < LED_NUM;i++){
                    tmp1 = (LEDEffectMask[LED_EFFECT_MASK_KEY_FUNC][i] * (LEDKeyPressEffectStartTime + LED_KEY_PRESS_EFFECT_TIME - millis) / LED_KEY_PRESS_EFFECT_TIME + LEDEffectMask[LED_EFFECT_MASK_DEFAULT][i] * (millis - LEDKeyPressEffectStartTime) / LED_KEY_PRESS_EFFECT_TIME);
                    for (j = 0; j < 3;j++,dat++){
                        *dat = (*dat) * tmp1 / 255;
                    }
                }
            break;
            case KEY_UP:
                for (i = 0,dat=(uint8*)LEDActData; i < LED_NUM;i++){
                    tmp1 = (LEDEffectMask[LED_EFFECT_MASK_KEY_UP][i] * (LEDKeyPressEffectStartTime + LED_KEY_PRESS_EFFECT_TIME - millis) / LED_KEY_PRESS_EFFECT_TIME + LEDEffectMask[LED_EFFECT_MASK_DEFAULT][i] * (millis - LEDKeyPressEffectStartTime) / LED_KEY_PRESS_EFFECT_TIME);
                    for (j = 0; j < 3;j++,dat++){
                        *dat = (*dat) * tmp1 / 255;
                    }
                }
            break;
        }
    }
    else{
        for (i = 0,dat=(uint8*)LEDActData; i < LED_NUM;i++){              
            for (j = 0; j < 3;j++,dat++){
                *dat = (*dat) * LEDEffectMask[LED_EFFECT_MASK_DEFAULT][i] / 255;
           }
        }
    }
    WS2812Update(LEDActData);
}

void bluetoothHandle(){
    if(UARTBTAvaliable()){
        playSound(NOTESEQ_SUCCESS);
        switch(UARTBTRead()){
            case '1':
                countdownData.left=300;
                countdownData.total=300;
                countdownData.running=true;
                nextInterface(INTERFACE_COUNTDOWN);
                break;
            case '2':
                countdownData.left=600;
                countdownData.total=600;
                countdownData.running=true;
                nextInterface(INTERFACE_COUNTDOWN);
                break;
            case '3':
                countdownData.left=900;
                countdownData.total=900;
                countdownData.running=true;
                nextInterface(INTERFACE_COUNTDOWN);
                break;
            case '4':
                WS2812Update(colorBasic[1]);
                LEDOn = false;
                break;
            case '5':
                LEDOn = true;
                break;
            case '6':
                LEDLightMode = false;
                break;
            case '7':
                LEDLightMode = true;
                break;
            case '8':
                LEDColor = 0;
                break;
            case '9':
                LEDColor = 1;
                break;

        }
    }

}


void timer4Interrupt() interrupt 20 {
    ++millis;
    localTimeUpdate();
    noteHandle();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//                                       真的不是Arduino！！！！你看那个void main()!!!!!!!!!!

void Delay1000ms(){		//@22.1184MHz
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 85;
	j = 12;
	k = 155;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void setup(){
    P3M0 &= 0xb3;
    P3M1 &= 0xb3;//准双向
    PIN_KEY_DOWN = 1;
    PIN_KEY_FUNC = 1;
    PIN_KEY_UP = 1;

    WS2812Init();

    I2CInit();
    UARTInit();
    graphicsInit();
    drawText(0, 0, "LOADING");
    //UARTDBSendStr("Init...\n");
    graphicsUpdate();
    Delay1000ms();//等待上电稳定...
    //EEPROMRecover();
    MP3PlayerInit();
    noteInit();
    AHT20Init();
    getTime(&nowTime);
    getHumiture(&nowHumiture);
    timer4Init();
    EA = 1;
    //UARTDBSendStr(VERSION_INFO);
    playSound(NOTESEQ_START_UP);
    MP3PlayerPlayTrack(NOTESEQ_START_UP);
}

void loop(){
    keyHandle();
    graphics();
    AHT20Handle();
    LEDHandle();
    standbyHandle();
    bluetoothHandle();
}

void main(){
    setup();
    for (;;){
        loop();
    }
}