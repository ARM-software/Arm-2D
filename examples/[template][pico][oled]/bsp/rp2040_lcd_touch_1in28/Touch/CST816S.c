/*****************************************************************************
 * | File      	:   CST816S.c
 * | Author      :   Waveshare team
 * | Function    :   Hardware underlying interface
 * | Info        :
 *                Used to shield the underlying layers of each master
 *                and enhance portability
 *----------------
 * |	This version:   V1.0
 * | Date        :   2022-12-02
 * | Info        :   Basic version
 *
 ******************************************************************************/
#include "CST816S.h"
CST816S Touch_CTS816;

void CST816S_I2C_Write(uint8_t reg, uint8_t value)
{
    DEV_I2C_Write_Byte(SENSOR_I2C_PORT, CST816_ADDR, reg, value);
}
uint8_t CST816S_I2C_Read(uint8_t reg)
{
    uint8_t res;
    res = DEV_I2C_Read_Byte(SENSOR_I2C_PORT, CST816_ADDR, reg);
    return res;
}

uint8_t CST816S_WhoAmI()
{
    if (CST816S_I2C_Read(CST816_ChipID) == 0xB5)
        return true;
    else
        return false;
}

void CST816S_Reset()
{
    DEV_Digital_Write(Touch_RST_PIN, 0);
    DEV_Delay_ms(100);
    DEV_Digital_Write(Touch_RST_PIN, 1);
    DEV_Delay_ms(100);
}

uint8_t CST816S_Read_Revision()
{
    return CST816S_I2C_Read(CST816_FwVersion);
}

void CST816S_Wake_up()
{
    DEV_Digital_Write(Touch_RST_PIN, 0);
    DEV_Delay_ms(10);
    DEV_Digital_Write(Touch_RST_PIN, 1);
    DEV_Delay_ms(50);
    CST816S_I2C_Write(CST816_DisAutoSleep, 0x01);
}

void CST816S_Stop_Sleep()
{
    CST816S_I2C_Write(CST816_DisAutoSleep, 0x01);
}

void CST816S_Set_Mode(uint8_t mode)
{
    if (mode == CST816S_Point_Mode)
    {
        // 
        CST816S_I2C_Write(CST816_IrqCtl, 0x41);
        
    }
    else if (mode == CST816S_Gesture_Mode)
    {
        CST816S_I2C_Write(CST816_IrqCtl, 0X11);
        CST816S_I2C_Write(CST816_MotionMask, 0x01);
    }
    else
    {
        CST816S_I2C_Write(CST816_IrqCtl, 0X71);
    }
        
}



uint8_t CST816S_init(uint8_t mode)
{
    uint8_t bRet, Rev;
    CST816S_Reset();

    bRet = CST816S_WhoAmI();
    if (bRet)
    {
        printf("Success:Detected CST816T.\r\n");
        Rev = CST816S_Read_Revision();
        printf("CST816T Revision = %d\r\n", Rev);
        CST816S_Stop_Sleep();
    }
    else
    {
        printf("Error: Not Detected CST816T.\r\n");
        return false;
    }

    CST816S_Set_Mode(mode);
    Touch_CTS816.x_point = 0;
    Touch_CTS816.y_point = 0;
    CST816S_I2C_Write(CST816_IrqPluseWidth, 0x01);
    CST816S_I2C_Write(CST816_NorScanPer, 0x01);
    
    Touch_CTS816.mode = mode;

    return true;
}

CST816S CST816S_Get_Point()
{
    uint8_t x_point_h, x_point_l, y_point_h, y_point_l;
    // CST816S_Wake_up();

    x_point_h = CST816S_I2C_Read(CST816_XposH);
    x_point_l = CST816S_I2C_Read(CST816_XposL);
    y_point_h = CST816S_I2C_Read(CST816_YposH);
    y_point_l = CST816S_I2C_Read(CST816_YposL);

    Touch_CTS816.x_point = ((x_point_h & 0x0f) << 8) + x_point_l;
    Touch_CTS816.y_point = ((y_point_h & 0x0f) << 8) + y_point_l;

    return Touch_CTS816;
}
uint8_t CST816S_Get_Gesture(void)
{
    uint8_t gesture;
    gesture=CST816S_I2C_Read(CST816_GestureID);
    return gesture;
}