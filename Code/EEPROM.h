#include "dataTypeDef.h"

void EEPROMEnable();
void EEPROMDisable();
void EEPROMWrite(uint16 addr, uint8 dat);
uint8 EEPROMRead(uint16 addr, uint8 dat);
void EEPROMErase(uint16 addr);