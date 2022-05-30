#include "dataTypeDef.h"

#define NOTESEQ_QIFENGLE 0
#define NOTESEQ_WEN      1
#define NOTESEQ_DEFAULT  2
#define NOTRSEQ_WARNINGS 3
#define NOTESEQ_START_UP 4
#define NOTESEQ_ERROR    5
#define NOTESEQ_SUCCESS  6
#define NOTESEQ_INCREASE 7
#define NOTESEQ_DECREASE 8
#define NOTESEQ_COUNTDOWN_OVER 9
#define NOTESEQ_BEEP     10

void noteHandle();
void playNoteSequence(uint8 n);
void noteInit();