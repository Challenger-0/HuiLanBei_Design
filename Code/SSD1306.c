#include "SSD1306.h"
void OLEDSendData(uint8 byte){
    I2CStart(0x3C, I2C_WRITE);
    I2CSend(0x40);
    I2CSend(byte);
    I2CEnd();
}

void OLEDSendCommand(uint8 cmd){
    I2CStart(0x3C, I2C_WRITE);
    I2CSend(0x00);
    I2CSend(cmd);
    I2CEnd();
}

void OLEDSendDataPackage(uint8  *dat,uint16 len){
    uint16 i = 0;
    OLEDDataPointerReset();
    I2CStart(0x3C, I2C_WRITE);
    I2CSend(0x40);
    for (i = 0; i < 1024;i++){
        I2CSend(dat[i]);
    }
    I2CEnd();
}

void OLEDFill(uint8 fillData){
    uint16 i = 0;
    OLEDDataPointerReset();
    I2CStart(0x3C, I2C_WRITE);
    I2CSend(0x40);
    for (i = 0; i < 1024;i++){
        I2CSend(fillData);
    }
    I2CEnd();
}
void OLEDSetContrast(uint8 contrast){
    OLEDSendCommand(0x81); //设置对比度
	OLEDSendCommand(contrast); //对比度,数值越大对比度越高
}

void OLEDInit(){
    OLEDSendCommand(OLED_CMD_DISPLAY_SWITCH | OFF);

    OLEDSendCommand(OLED_CMD_MEMORY_ADDRESSING_MODE);
    OLEDSendCommand(HORIZONTAL);
	
    OLEDSendCommand(0x81); //设置对比度
	OLEDSendCommand(0xff); //对比度,数值越大对比度越高
    OLEDSendCommand(0xa1); //设置段重新映射 不左右翻转set segment remap
    OLEDSendCommand(0xa6); //设置正常/反向 normal / reverse
    OLEDSendCommand(0xa8); //设置多路复用比(1-64) set multiplex ratio(1 to 64)
    OLEDSendCommand(0x3f); //设定值1/32  1/32 duty
    OLEDSendCommand(0xc8); //扫描方向 不上下翻转Com scan direction
    OLEDSendCommand(0xd3); //设置显示偏移 set display offset
    OLEDSendCommand(0x00); //
    OLEDSendCommand(0xd5); //设置osc分区 set osc division
    OLEDSendCommand(0x80); //
    OLEDSendCommand(0xd8); //关闭区域颜色模式 set area color mode off
    OLEDSendCommand(0x05); //
    OLEDSendCommand(0xd9); //设置预充电期 Set Pre-Charge Period
    OLEDSendCommand(0xf1); //
    OLEDSendCommand(0xda); //设置com引脚配置 set com pin configuartion
    OLEDSendCommand(0x12); //
    OLEDSendCommand(0xdb); //设置vcomh set Vcomh
    OLEDSendCommand(0x30); //
    OLEDSendCommand(0x8d); //设置电源泵启用 set charge pump enable
    OLEDSendCommand(0x14); //
    OLEDSendCommand(0xa4); //设置全局显示开启；bit0，1开启，0关闭(白屏/黑屏)

    OLEDSendCommand(OLED_CMD_DISPLAY_SWITCH | ON);
	
	
}

void OLEDDataPointerReset(){
    OLEDSendCommand(OLED_CMD_COLUMN_ADDRESS);
    OLEDSendCommand(0);
    OLEDSendCommand(127);
    OLEDSendCommand(OLED_CMD_PAGE_ADDRESS);
    OLEDSendCommand(0);
    OLEDSendCommand(7);
}