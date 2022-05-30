#include "note.h"
#include "Timer.h"

#define A_NOTE   0
#define H2_NOTE  1
#define H4_NOTE  2
#define H8_NOTE  3
#define H16_NOTE 4
#define H32_NOTE 5
#define H64_NOTE 6

#define SPEED_60  0
#define SPEED_80  1
#define SPEED_100 2
#define SPEED_120 3
#define SPEED_140 4
#define SPEED_160 5
#define SPEED_180 6

#define BUZZER_PIN P37  

const uint16 code NOTE_SPEED[7][7] = {
    //    1    1/2  1/4  1/8  1/16  1/32  1/64
    {1000, 500, 250, 125, 63, 31, 16},
    {750, 375, 188, 94, 47, 23, 11},
    {600, 300, 150, 75, 38, 19, 9},
    {500, 250, 125, 63, 31, 16, 8},
    {429, 214, 107, 54, 27, 13, 7},
    {375, 189, 94, 47, 23, 12, 6},
    {333, 167, 83, 42, 21, 10, 5}
};

#define NOTE_END  -2
#define NOTE_REST -1
#define NOTE_C6  0
#define NOTE_C6H 1
#define NOTE_D6  2
#define NOTE_D6H 3
#define NOTE_E6  4
#define NOTE_F6  5
#define NOTE_F6H 6
#define NOTE_G6  7
#define NOTE_G6H 8
#define NOTE_A6  9
#define NOTE_A6H 10
#define NOTE_B6  11
#define NOTE_C7  12
#define NOTE_C7H 13
#define NOTE_D7  14
#define NOTE_D7H 15
#define NOTE_E7  16
#define NOTE_F7  17
#define NOTE_F7H 18
#define NOTE_G7  19
#define NOTE_G7H 20
#define NOTE_A7  21
#define NOTE_A7H 22
#define NOTE_B7  23
#define NOTE_C8  24
#define NOTE_C8H 25
#define NOTE_D8  26
#define NOTE_D8H 27
#define NOTE_E8  28
#define NOTE_F8  29
#define NOTE_F8H 30
#define NOTE_G8  31
#define NOTE_G8H 32
#define NOTE_A8  33
#define NOTE_A8H 34
#define NOTE_B8  35

const uint8 code RELOAD_H[] = {0xD6,0xD9,0xdb,0xdd,0xdf,0xe1,0xe2,0xe4,0xe5,0xe7,0xe8,0xea,0xeb,0xec,0xed,0xee,0xef,0xf0,0xf1,0xf2,0xf3,0xf3,0xf4,0xf5,0xf5,0xf6,0xf6,0xf7,0xf7,0xf8,0xf8,0xf9,0xf9,0xf9,0xfa,0xfa};
const uint8 code RELOAD_L[] = {0xB3,0x09,0x32,0x44,0x41,0x12,0xcc,0x70,0xfe,0x76,0xd8,0x24,0x5a,0x79,0x99,0xa2,0x96,0x89,0x66,0x43,0x0a,0xbb,0x6c,0x07,0xb8,0x3d,0xd7,0x5c,0xcb,0x4f,0xbe,0x17,0x85,0xde,0x36,0x8f};

typedef struct{
    int8 n;
    uint8 t;
}note;

typedef struct{
    uint8 speed;
    note code *sequence;
} noteSequence;

note code NOTE_QIFENGLE[] = {{NOTE_C7,H2_NOTE},{NOTE_D7,H2_NOTE},{NOTE_E7,H2_NOTE},{NOTE_A7,H4_NOTE},{NOTE_G7,H4_NOTE},{NOTE_G7,H2_NOTE},{NOTE_A7,H4_NOTE},{NOTE_G7,H4_NOTE},{NOTE_G7,H2_NOTE},{NOTE_A7,H4_NOTE},{NOTE_G7,H4_NOTE},{NOTE_G7,H2_NOTE},{NOTE_D7,H2_NOTE},{NOTE_E7,H2_NOTE},{NOTE_A7,H4_NOTE},{NOTE_G7,H4_NOTE},{NOTE_G7,H2_NOTE},{NOTE_A7,H4_NOTE},{NOTE_G7,H4_NOTE},{NOTE_G7,H2_NOTE},{NOTE_A7,H4_NOTE},{NOTE_G7,H4_NOTE},{NOTE_G7,H2_NOTE},{NOTE_E7,H2_NOTE},{NOTE_D7,H2_NOTE},{NOTE_C7,H4_NOTE},{NOTE_A6,H4_NOTE},{NOTE_A6,H2_NOTE},{NOTE_C7,H2_NOTE},{NOTE_D7,H2_NOTE},{NOTE_C7,H4_NOTE},{NOTE_A6,H4_NOTE},{NOTE_A6,H2_NOTE},{NOTE_C7,H2_NOTE},{NOTE_E7,A_NOTE},{NOTE_E7,H4_NOTE},{NOTE_F7,H4_NOTE},{NOTE_E7,H2_NOTE},{NOTE_D7,A_NOTE},{NOTE_END,NOTE_A6H}};
note code NOTE_WEN[] =      {{NOTE_D7,H2_NOTE},{NOTE_E7,H2_NOTE},{NOTE_G7,H2_NOTE},{NOTE_E7,H2_NOTE},{NOTE_D7,H2_NOTE},{NOTE_REST,H64_NOTE},{NOTE_D7,H2_NOTE},{NOTE_E7,H2_NOTE},{NOTE_D7,H2_NOTE},{NOTE_D7,H4_NOTE},{NOTE_C7,H4_NOTE},{NOTE_D7,A_NOTE},{NOTE_REST,A_NOTE},{NOTE_D7,H2_NOTE},{NOTE_E7,H2_NOTE},{NOTE_G7,H2_NOTE},{NOTE_D7,H2_NOTE},{NOTE_G6,A_NOTE},{NOTE_D7,H2_NOTE},{NOTE_B6,A_NOTE},{NOTE_E7,H2_NOTE},{NOTE_E7,A_NOTE},{NOTE_C7,A_NOTE},{NOTE_REST,H2_NOTE},{NOTE_A6,H4_NOTE},{NOTE_C7,H4_NOTE},{NOTE_D7,H2_NOTE},{NOTE_E7,H2_NOTE},{NOTE_G7,H2_NOTE},{NOTE_A7,H2_NOTE},{NOTE_D7,A_NOTE},{NOTE_REST,H2_NOTE},{NOTE_E7,H2_NOTE},{NOTE_D7,H2_NOTE},{NOTE_C7,H2_NOTE},{NOTE_B6,H2_NOTE},{NOTE_C7,H2_NOTE},{NOTE_A6,A_NOTE},{NOTE_A6,A_NOTE},{NOTE_END,A_NOTE}};
note code NOTE_DEFAULT[] =  {{NOTE_C7, H32_NOTE}, {NOTE_END, A_NOTE}};
note code NOTE_WARNINGS[] = {{NOTE_D6,H16_NOTE}, {NOTE_END, A_NOTE}};
note code NOTE_START_UP[] = {{NOTE_G6,H16_NOTE},{NOTE_REST,H16_NOTE},{NOTE_C7,H16_NOTE}, {NOTE_END, A_NOTE}};
note code NOTE_ERROR   [] = {{NOTE_E6,H16_NOTE},{NOTE_REST,H16_NOTE},{NOTE_E6,H16_NOTE},{NOTE_REST,H16_NOTE},{NOTE_E6,H16_NOTE},{NOTE_REST,H16_NOTE},{NOTE_E6,H16_NOTE},{NOTE_REST,H16_NOTE}, {NOTE_END, A_NOTE}};
note code NOTE_SUCCESS[] =  {{NOTE_G7, H16_NOTE}, {NOTE_REST, H16_NOTE}, {NOTE_G7, H16_NOTE}, {NOTE_END, A_NOTE}};
note code NOTE_INCREASE[] = {{NOTE_C7, H16_NOTE}, {NOTE_G7, H16_NOTE}, {NOTE_END, A_NOTE}};
note code NOTE_DECREASE[] = {{NOTE_C7, H16_NOTE}, {NOTE_G6, H16_NOTE}, {NOTE_END, A_NOTE}};
note code NOTE_COUNTDOWN_OVER[] = {{NOTE_C6,H8_NOTE},{NOTE_D6,H8_NOTE},{NOTE_G6,H8_NOTE},{NOTE_C7,H8_NOTE},{NOTE_D7,H8_NOTE},{NOTE_G7,H8_NOTE},{NOTE_D8,H8_NOTE},{NOTE_G8,H8_NOTE}, {NOTE_END, A_NOTE}};
note code NOTE_BEEP[] = {{NOTE_C7,A_NOTE},{NOTE_REST,A_NOTE},{NOTE_C7,A_NOTE},{NOTE_REST,A_NOTE},{NOTE_C7,A_NOTE},{NOTE_REST,A_NOTE},{NOTE_C7,A_NOTE},{NOTE_REST,A_NOTE},{NOTE_C7,A_NOTE},{NOTE_REST,A_NOTE},{NOTE_C7,A_NOTE},{NOTE_REST,A_NOTE},{NOTE_C7,A_NOTE},{NOTE_REST,A_NOTE},{NOTE_C7,A_NOTE},{NOTE_REST,A_NOTE},{NOTE_END,A_NOTE}};



noteSequence code NOTE_SEQUENCE_QIFENGLE = {SPEED_80,NOTE_QIFENGLE};
noteSequence code NOTE_SEQUENCE_WEN =      {SPEED_80,NOTE_WEN};
noteSequence code NOTE_SEQUENCE_DEFAULT =  {SPEED_60, NOTE_DEFAULT};
noteSequence code NOTE_SEQUENCE_WARNINGS = {SPEED_60, NOTE_WARNINGS};
noteSequence code NOTE_SEQUENCE_START_UP = {SPEED_60, NOTE_START_UP};
noteSequence code NOTE_SEQUENCE_ERROR = {SPEED_60, NOTE_ERROR};
noteSequence code NOTE_SEQUENCE_SUCCESS  = {SPEED_60, NOTE_SUCCESS};
noteSequence code NOTE_SEQUENCE_INCREASE = {SPEED_60, NOTE_INCREASE};
noteSequence code NOTE_SEQUENCE_DECREASE = {SPEED_60, NOTE_DECREASE};
noteSequence code NOTE_SEQUENCE_COUNTDOWN_OVER = {SPEED_60, NOTE_COUNTDOWN_OVER};
noteSequence code NOTE_SEQUENCE_BEEP = {SPEED_60, NOTE_BEEP};

noteSequence code *code NOTE_SEQUENCE_LIST[] = {&NOTE_SEQUENCE_QIFENGLE,&NOTE_SEQUENCE_WEN,&NOTE_SEQUENCE_DEFAULT,&NOTE_SEQUENCE_WARNINGS,&NOTE_SEQUENCE_START_UP,&NOTE_SEQUENCE_ERROR,&NOTE_SEQUENCE_SUCCESS,&NOTE_SEQUENCE_INCREASE,&NOTE_SEQUENCE_DECREASE,&NOTE_SEQUENCE_COUNTDOWN_OVER,&NOTE_BEEP};

uint8 xdata notePlaying = false;
uint16 xdata notePlayingIndex = 0;
uint16 xdata notePlayingCount = 0;
noteSequence code * xdata nowPlayingNoteSequence;

void playNote(note code *n){
    TR0 = 0;
    ET0 = 0;
    notePlayingCount = 0;
    BUZZER_PIN = 1;
    if(n->n==NOTE_END){
        return;
    }
    if(n->n==NOTE_REST){
        notePlaying = true;
        return;
    }
    TH0 = RELOAD_H[n->n];
    TL0 = RELOAD_L[n->n];
    TR0 = 1;
    ET0 = 1;
    notePlaying = true;
}

void noteHandle(){
    if (notePlaying){
        notePlayingCount++;
        if (notePlayingCount == NOTE_SPEED[nowPlayingNoteSequence->speed][(nowPlayingNoteSequence->sequence[notePlayingIndex]).t]){
            notePlaying = false;
            notePlayingCount = 0;
            playNote(&(nowPlayingNoteSequence->sequence[++notePlayingIndex]));
        }
    }
}
void noteStop(){
    TR0 = 0;
    ET0 = 0;
    notePlaying = false;

}

void playNoteSequence(uint8 n){
    TR0 = 0;
    ET0 = 0;
    notePlaying = false;
    nowPlayingNoteSequence = NOTE_SEQUENCE_LIST[n];
    notePlayingIndex = 0;
    playNote(NOTE_SEQUENCE_LIST[n]->sequence);
}

void noteInit(){
    AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TF0 = 0;		//清除TF0标志
    ET0 = 1;

    // CMOD=0x04;
    // CCAPM2 = 0x42;//通道2PWM模式
    // PCA_PWM2 = 0x80;//6位，

    P3M0 &= ~1 << 7;
    P3M1 &= ~1 << 7;//准双向
}

void noteInterrupt() interrupt 1{
    BUZZER_PIN = !BUZZER_PIN;
} 