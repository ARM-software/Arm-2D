/*****************************************************************************
* | File        :   OLED_1in3_c.c
* | Author      :   
* | Function    :   1.3inch OLED  Drive function
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2021-03-16
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
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
#include "OLED_1in3_c.h"
#include "stdio.h"
#include <stdint.h>

/*******************************************************************************
function:
			Hardware reset
*******************************************************************************/
static void I2C_Write_Byte( uint8_t Value,uint8_t reg)
{
    DEV_I2C_Write(0x3c, reg, Value);
}

static void OLED_Reset(void)
{
    OLED_RST_1;
    DEV_Delay_ms(100);
    OLED_RST_0;
    DEV_Delay_ms(100);
    OLED_RST_1;
    DEV_Delay_ms(100);
}

/*******************************************************************************
function:
			Write register address and data
*******************************************************************************/
void OLED_WriteReg(uint8_t Reg)
{
#if USE_SPI
    OLED_DC_0;
    OLED_CS_0;
    DEV_SPI_WriteByte(Reg);
    OLED_CS_1;
#elif USE_IIC
    I2C_Write_Byte(Reg,IIC_CMD);
#endif
}

void OLED_WriteData(uint8_t Data)
{	
#if USE_SPI
    OLED_DC_1;
    OLED_CS_0;
    DEV_SPI_WriteByte(Data);
    OLED_CS_1;
#elif USE_IIC
    I2C_Write_Byte(Data,IIC_RAM);
#endif
}

/*******************************************************************************
function:
			Common register initialization
*******************************************************************************/
static void OLED_InitReg(void)
{
    OLED_WriteReg(0xae);	/*turn off OLED display*/

    OLED_WriteReg(0x00);	/*set lower column address*/ 
    OLED_WriteReg(0x10);	/*set higher column address*/ 

    OLED_WriteReg(0xB0);	/*set page address*/ 
	
    OLED_WriteReg(0xdc);	/*set display start line*/ 
    OLED_WriteReg(0x00);  

    OLED_WriteReg(0x81);	/*contract control*/ 
    OLED_WriteReg(0x6f);	/*128*/ 
    OLED_WriteReg(0x21);  	/* Set Memory addressing mode (0x20/0x21) */ 
	
    OLED_WriteReg(0xa0); 	/*set segment remap*/ 
    OLED_WriteReg(0xc0);  	/*Com scan direction*/ 
    OLED_WriteReg(0xa4);	/*Disable Entire Display On (0xA4/0xA5)*/ 

    OLED_WriteReg(0xa6);	/*normal / reverse*/
    OLED_WriteReg(0xa8);	/*multiplex ratio*/ 
    OLED_WriteReg(0x3f);  	/*duty = 1/64*/ 
  
    OLED_WriteReg(0xd3);  	/*set display offset*/ 
    OLED_WriteReg(0x60);

    OLED_WriteReg(0xd5);	/*set osc division*/ 
    OLED_WriteReg(0x41);
		
    OLED_WriteReg(0xd9); 	/*set pre-charge period*/ 
    OLED_WriteReg(0x22);   

    OLED_WriteReg(0xdb); 	/*set vcomh*/ 
    OLED_WriteReg(0x35);  

    OLED_WriteReg(0xad); 	/*set charge pump enable*/ 
    OLED_WriteReg(0x8a);	/*Set DC-DC enable (a=0:disable; a=1:enable) */
}


/********************************************************************************
function:
			initialization
********************************************************************************/
void OLED_1in3_C_Init()
{
    //Hardware reset
    OLED_Reset();

    //Set the initialization register
    OLED_InitReg();
    DEV_Delay_ms(200);

    //Turn on the OLED display
    OLED_WriteReg(0xaf);
}


/********************************************************************************
function:
			Clear screen
********************************************************************************/
void OLED_1in3_C_Clear(void)
{
	UWORD Width, Height, column;
	Width = (OLED_1in3_C_WIDTH % 8 == 0)? (OLED_1in3_C_WIDTH / 8 ): (OLED_1in3_C_WIDTH / 8 + 1);
	Height = OLED_1in3_C_HEIGHT;  
	OLED_WriteReg(0xb0); 	//Set the row  start address
	for (UWORD j = 0; j < Height; j++) {
		column = 63 - j;
		OLED_WriteReg(0x00 + (column & 0x0f));  //Set column low start address
		OLED_WriteReg(0x10 + (column >> 4));  //Set column higt start address
		for (UWORD i = 0; i < Width; i++) {
			OLED_WriteData(0x00);
		 }
	}
}

/********************************************************************************
function:   
            reverse a byte data
********************************************************************************/
static UBYTE reverse(UBYTE temp)
{
    temp = ((temp & 0x55) << 1) | ((temp & 0xaa) >> 1);
    temp = ((temp & 0x33) << 2) | ((temp & 0xcc) >> 2);
    temp = ((temp & 0x0f) << 4) | ((temp & 0xf0) >> 4);  
    return temp;
}

/********************************************************************************
function:	
			Update all memory to OLED
********************************************************************************/
void OLED_1in3_C_Display(const UBYTE *Image)
{		
    UWORD Width, Height, column, temp;
    Width = (OLED_1in3_C_WIDTH % 8 == 0)? (OLED_1in3_C_WIDTH / 8 ): (OLED_1in3_C_WIDTH / 8 + 1);
    Height = OLED_1in3_C_HEIGHT;   
    OLED_WriteReg(0xb0); 	//Set the row  start address
    for (UWORD j = 0; j < Height; j++) {
        column = 63 - j;
        OLED_WriteReg(0x00 + (column & 0x0f));  //Set column low start address
        OLED_WriteReg(0x10 + (column >> 4));  //Set column higt start address
        for (UWORD i = 0; i < Width; i++) {
            temp = Image[i + j * Width];
            // printf("0x%x \r\n",temp);
            temp = reverse(temp);	//reverse the buffer
            OLED_WriteData(temp);
         }
    }   
}

void OLED_DrawBitmap(int16_t iX, int16_t iY, int16_t iWidth, int16_t iHeight, const uint8_t *pchBuffer)
{
    assert((iX & 0x7) == 0);
    assert((iWidth & 0x7) == 0);
    
    int16_t iColoumOffset = ((iX >> 3) & 0x0F);
    
    for (int16_t i = 0; i < iHeight; i++) {
    
        /* set row */
        int16_t iRow = GLCD_HEIGHT - (i + iY) - 1;
        OLED_WriteReg(0x00 + (iRow & 0x0f));
        OLED_WriteReg(0x10 + (iRow >> 4));

        OLED_WriteReg(0xb0 | iColoumOffset);   //! set page address
        
        
        for (int16_t j = 0; j < iWidth; j+=8) {
            uint8_t chData = 0;
            
            chData |= *pchBuffer++ >= 0x80 ? 0x80 : 0x00;
            chData >>= 1;
            
            chData |= *pchBuffer++ >= 0x80 ? 0x80 : 0x00;
            chData >>= 1;
            
            chData |= *pchBuffer++ >= 0x80 ? 0x80 : 0x00;
            chData >>= 1;
            
            chData |= *pchBuffer++ >= 0x80 ? 0x80 : 0x00;
            chData >>= 1;
            
            chData |= *pchBuffer++ >= 0x80 ? 0x80 : 0x00;
            chData >>= 1;
            
            chData |= *pchBuffer++ >= 0x80 ? 0x80 : 0x00;
            chData >>= 1;
            
            chData |= *pchBuffer++ >= 0x80 ? 0x80 : 0x00;
            chData >>= 1;
            
            chData |= *pchBuffer++ >= 0x80 ? 0x80 : 0x00;
            
            OLED_WriteData(chData);
        
        }
    }
    
}

void Disp0_DrawBitmap(  int16_t x, 
                        int16_t y, 
                        int16_t width, 
                        int16_t height, 
                        const uint8_t *bitmap)
{
    OLED_DrawBitmap(x, y, width, height, bitmap);
}


