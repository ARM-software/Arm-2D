/****************************************************************************
*  Copyright 2021 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/

/*============================ INCLUDES ======================================*/

#include <stdint.h>
#include <stdbool.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * BSP default APIs                                                           *
 *----------------------------------------------------------------------------*/
__attribute__((weak))
void breath_led(void) {
}


__attribute__((weak))
void bsp_init(void) {
}

__attribute__((weak))
void GLCD_DrawBitmap(   int_fast16_t x, int_fast16_t y, 
                        int_fast16_t width, int_fast16_t height,
                        uint16_t *frame_ptr)
{

}

__attribute__((weak))
void Disp0_DrawBitmap(  int16_t x, 
                        int16_t y, 
                        int16_t width, 
                        int16_t height, 
                        const uint8_t *bitmap)
{
}

__attribute__((weak))
void DEV_Digital_Write(uint16_t Pin, uint8_t Value) {}

__attribute__((weak))
uint8_t DEV_Digital_Read(uint16_t Pin) 
{ 
    return 0; 
}

__attribute__((weak))
void DEV_GPIO_Mode(uint16_t Pin, uint16_t Mode) {}

__attribute__((weak))
void dev_key_init(uint8_t chPin) {}

__attribute__((weak))
bool dev_read_key(uint8_t chPin) 
{ 
    return false;
}

__attribute__((weak))
void DEV_SPI_WriteByte(uint8_t Value) {}

__attribute__((weak))
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len) {}

__attribute__((weak))
void DEV_Delay_ms(uint32_t xms) 
{ 
    
}

__attribute__((weak))
void DEV_Delay_us(uint32_t xus) {
    
}

__attribute__((weak))
void DEV_I2C_Write(uint8_t addr, uint8_t reg, uint8_t Value) {}

__attribute__((weak))
void DEV_I2C_Write_nByte(uint8_t addr, uint8_t *pData, uint32_t Len) {}

__attribute__((weak))
uint8_t DEV_I2C_ReadByte(uint8_t addr, uint8_t reg)
{
    return 0;
}

__attribute__((weak))
void DEV_SET_PWM(uint8_t Value) {}

__attribute__((weak))
uint8_t DEV_Module_Init(void) 
{
    return 0;
}

__attribute__((weak))
void DEV_Module_Exit(void) {}

