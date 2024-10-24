/*****************************************************************************
* | File        :   OLED_2in23_c.c
* | Author      :   
* | Function    :   2.23inch OLED  Drive function
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
#include "OLED_2in23.h"
#include "stdio.h"

static uint8_t s_chDispalyBuffer[128][4];

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
    OLED_CS_1;
	OLED_DC_0;
    OLED_RST_1;
    DEV_Delay_ms(100);
}

/*******************************************************************************
function:
			Write register address and data
*******************************************************************************/
static void OLED_WriteReg(uint8_t Reg)
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

static void OLED_WriteData(uint8_t Data)
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
    OLED_WriteReg(0xAE);	/*turn off OLED display*/
    
    OLED_WriteReg(0x04);	/*turn off OLED display*/

    OLED_WriteReg(0x10);	/*turn off OLED display*/	
	
    OLED_WriteReg(0x40);	/*set lower column address*/ 
    OLED_WriteReg(0x81);	/*set higher column address*/ 
    OLED_WriteReg(0x80);  	//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F, SSD1305_CMD)
    OLED_WriteReg(0xA1);	//--set contrast control register
    OLED_WriteReg(0xA6);	// Set SEG Output Current Brightness ��ת 
	OLED_WriteReg(0xA8);  	//--Set SEG/Column Mapping	
    OLED_WriteReg(0x1F); 	//Set COM/Row Scan Direction   
    OLED_WriteReg(0xC0);  	//--set normal display  ����ɨ�跴�� 
	OLED_WriteReg(0xD3);	//--set multiplex ratio(1 to 64)
    OLED_WriteReg(0x00);	//--1/64 duty
    OLED_WriteReg(0xD5);	//-set display offset	Shift Mapping RAM Counter (0x00~0x3F) 
    OLED_WriteReg(0xF0);  	//-not offset
    OLED_WriteReg(0xD8);  	//--set display clock divide ratio/oscillator frequency
    OLED_WriteReg(0x05);	//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WriteReg(0xD9);	//--set pre-charge period
    OLED_WriteReg(0xC2);	//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WriteReg(0xDA); 	//--set com pins hardware configuration 
    OLED_WriteReg(0x12);   
    OLED_WriteReg(0xDB); 	 /*set vcomh*/
    OLED_WriteReg(0x08);  	//Set VCOM Deselect Level

    OLED_WriteReg(0xAF); 	//-Set Page Addressing Mode (0x00/0x01/0x02)
   	OLED_2in23_Clear(0x00);
   
}


/********************************************************************************
function:
			initialization
********************************************************************************/
void OLED_2in23_Init()
{
    //Hardware reset
    OLED_Reset();

    //Set the initialization register
    OLED_InitReg();
    DEV_Delay_ms(200);

    //Turn on the OLED display
	OLED_WriteReg(0xAF);  
}


/********************************************************************************
function:
			Clear screen
********************************************************************************/
void OLED_2in23_refresh_gram()
{
	UWORD i, j;
	
	for (i = 0; i < 4; i ++) {  
		OLED_WriteReg(0xB0 + i);   
		OLED_WriteReg(0x04); 
		OLED_WriteReg(0x10);    
		for (j = 0; j < 128; j ++) {
			
			OLED_WriteData(s_chDispalyBuffer[j][i]); 
		}
	}   
}




void OLED_2in23_Clear(uint8_t chFill)
{
	uint8_t i, j;
	
	for (i = 0; i < 4; i ++) {
		OLED_WriteReg(0xB0 + i);
		OLED_WriteReg(0x04); 
		OLED_WriteReg(0x10); 
		for (j = 0; j < 128; j ++) {
			s_chDispalyBuffer[j][i] = chFill;
		}
	}
	
	OLED_2in23_refresh_gram();
}

/********************************************************************************
function:	
			Update all memory to OLED
********************************************************************************/

void OLED_2in23_draw_point(uint8_t chXpos, uint8_t chYpos, uint8_t chPoint)
{
	uint8_t chPos, chBx, chTemp = 0;
	
	if (chXpos > 127 || chYpos > 31) {
		return;
	}
	chPos = 7 - chYpos / 8; // 
	chBx = chYpos % 8;
	chTemp = 1 << (7 - chBx);
	
	if (chPoint) {
		s_chDispalyBuffer[chXpos][chPos] |= chTemp;
		
	} else {
		s_chDispalyBuffer[chXpos][chPos] &= ~chTemp;
	}
	
}


void OLED_2in23_draw_bitmap(unsigned char x,unsigned char y,const unsigned char *pBmp,
					unsigned char chWidth,unsigned char chHeight)
{
	uint16_t i, j, byteWidth = (chWidth + 7)/8;
    for(j = 0; j < chHeight; j ++){
        for(i = 0; i < chWidth; i ++ ) {
            if(*(pBmp + j * byteWidth + i / 8) & (128 >> (i & 7))) {
                OLED_2in23_draw_point(x+i, y+j, 1);
            }else {
                OLED_2in23_draw_point(x+i, y+j, 0);
            }
        }
    }
    OLED_2in23_refresh_gram();    
}

      


