#include "I2C.h"
#include "dataTypeDef.h"

#define ON 1
#define OFF 0

#define HORIZONTAL      0
#define VERTICAL        1
#define PAGE_ADDERSSING 2

#define OLED_CMD_CONTRAST 0x81
#define OLED_CMD_ENTIRE_DISPLAY 0xA4
#define OLED_CMD_DISPLAY_INVERSE 0xA6
#define OLED_CMD_DISPLAY_SWITCH 0xAE
#define OLED_CMD_MEMORY_ADDRESSING_MODE 0x20
#define OLED_CMD_COLUMN_ADDRESS 0x21
#define OLED_CMD_PAGE_ADDRESS 0x22

void OLEDSendData(uint8 byte);
void OLEDSendCommand(uint8 cmd);
void OLEDSendDataPackage(uint8 *dat, uint16 len);
void OLEDFill(uint8 fillData);
void OLEDInit();
void OLEDDataPointerReset();
void OLEDSetContrast(uint8 contrast);