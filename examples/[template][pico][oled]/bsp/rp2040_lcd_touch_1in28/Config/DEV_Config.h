/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2021-03-16
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of theex Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"


#define LCD_SPI_PORT    (spi1)
#define SENSOR_I2C_PORT (i2c1)
/**
 * GPIOI config
 **/
#define DEV_SDA_PIN     (6)
#define DEV_SCL_PIN     (7)

#define LCD_DC_PIN      (8)
#define LCD_CS_PIN      (9)
#define LCD_CLK_PIN     (10)
#define LCD_MOSI_PIN    (11)
#define LCD_MISO_PIN    (12)
#define LCD_RST_PIN     (13)
#define LCD_BL_PIN      (25)

#define Touch_INT_PIN   (21)
#define Touch_RST_PIN   (22)

#define DOF_INT1        (23)
#define DOF_INT2        (24)

#define BAT_ADC_PIN     (29)
#define BAR_CHANNEL     (3)

/*------------------------------------------------------------------------------------------------------*/

void DEV_Delay_ms(uint32_t xms);
void DEV_Delay_us(uint32_t xus);

void DEV_Digital_Write(uint16_t Pin, uint8_t Value);
uint8_t DEV_Digital_Read(uint16_t Pin);

void DEV_GPIO_Mode(uint16_t Pin, uint16_t Mode);
void DEV_KEY_Config(uint16_t Pin);
void DEV_Digital_Write(uint16_t Pin, uint8_t Value);
uint8_t DEV_Digital_Read(uint16_t Pin);

uint16_t DEC_ADC_Read(void);

void DEV_SPI_WriteByte(spi_inst_t *SPI_PORT,uint8_t Value);
void DEV_SPI_Write_nByte(spi_inst_t *SPI_PORT,uint8_t *pData, uint32_t Len);


void DEV_I2C_Write_Byte(i2c_inst_t *I2C_PORT,uint8_t addr, uint8_t reg, uint8_t Value);
void DEV_I2C_Write_nByte(i2c_inst_t *I2C_PORT,uint8_t addr, uint8_t *pData, uint32_t Len);
uint8_t DEV_I2C_Read_Byte(i2c_inst_t *I2C_PORT,uint8_t addr, uint8_t reg);
void DEV_I2C_Read_nByte(i2c_inst_t *I2C_PORT,uint8_t addr,uint8_t reg, uint8_t *pData, uint32_t Len);

void DEV_IRQ_SET(uint gpio, uint32_t events, gpio_irq_callback_t callback);

void DEV_SET_PWM(uint8_t Value);

uint8_t DEV_Module_Init(void);
void DEV_Module_Exit(void);

#endif
