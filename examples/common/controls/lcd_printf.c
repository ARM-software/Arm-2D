/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*============================ INCLUDES ======================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "arm_extra_lcd_printf.h"
#include "arm_2d.h"
#include "./__common.h"

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wformat-nonliteral"
#   pragma clang diagnostic ignored "-Wsign-compare"
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpedantic"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#endif

/*============================ MACROS ========================================*/

#ifndef __LCD_PRINTF_CFG_TEXT_BUFFER_SIZE__
#   define __LCD_PRINTF_CFG_TEXT_BUFFER_SIZE__              256
#endif

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern
const uint8_t Font_6x8_h[(144-32)*8];


/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static struct {
    arm_2d_region_t tRegion;
    struct {
        uint8_t         chX;
        uint8_t         chY;
    } tTextLocation;
    
    struct {
        COLOUR_INT_TYPE     tForeground;
        COLOUR_INT_TYPE     tBackground;
    } tColour;
    
    arm_2d_tile_t *ptTargetFB;
    uint32_t       wMode;
    
    const arm_2d_font_t *ptFont;
    arm_2d_char_descriptor_t tCharDescriptor;
    
} s_tLCDTextControl = {
    .tRegion = { 
        .tSize = {
            .iWidth = __GLCD_CFG_SCEEN_WIDTH__,
            .iHeight = __GLCD_CFG_SCEEN_HEIGHT__,
        },
    },
    .tColour = {
        .tForeground = GLCD_COLOR_GREEN,
        .tBackground = GLCD_COLOR_BLACK,
    },
    .ptTargetFB = (arm_2d_tile_t *)(-1),
    .wMode = ARM_2D_DRW_PATN_MODE_COPY,
            //| ARM_2D_DRW_PATN_MODE_NO_FG_COLOR    
            //| ARM_2D_DRW_PATH_MODE_COMP_FG_COLOUR 
    .ptFont = &ARM_2D_FONT_6x8.use_as__arm_2d_font_t,
    
};

/*============================ IMPLEMENTATION ================================*/

void arm_lcd_text_set_colour(COLOUR_INT_TYPE wForeground, COLOUR_INT_TYPE wBackground)
{
    s_tLCDTextControl.tColour.tForeground = wForeground;
    s_tLCDTextControl.tColour.tBackground = wBackground;
}

void arm_lcd_text_set_target_framebuffer(arm_2d_tile_t *ptFrameBuffer)
{
    s_tLCDTextControl.ptTargetFB = ptFrameBuffer;

    if (NULL == ptFrameBuffer) {
        // use default framebuffer
        s_tLCDTextControl.ptTargetFB = (arm_2d_tile_t *)(-1);
    }
}

void arm_lcd_text_set_display_mode(uint32_t wMode)
{
    s_tLCDTextControl.wMode = wMode;
}

void arm_lcd_text_set_draw_region(arm_2d_region_t *ptRegion)
{
    if (NULL == ptRegion) {
        ptRegion =  (arm_2d_region_t []) {
                    {
                        .tSize = {
                            .iWidth = __GLCD_CFG_SCEEN_WIDTH__,
                            .iHeight = __GLCD_CFG_SCEEN_HEIGHT__,
                        },
                    }};
    }
    
    s_tLCDTextControl.tRegion = *ptRegion;
}

void arm_lcd_text_location(uint8_t chY, uint8_t chX)
{
    s_tLCDTextControl.tTextLocation.chX = chX;
    s_tLCDTextControl.tTextLocation.chY = chY;
    
    if (    s_tLCDTextControl.tTextLocation.chX *  s_tLCDTextControl.ptFont->tCharSize.iWidth
        >=  s_tLCDTextControl.tRegion.tSize.iWidth ) {
        s_tLCDTextControl.tTextLocation.chX = 0;
        s_tLCDTextControl.tTextLocation.chY++;
        if (    s_tLCDTextControl.tTextLocation.chY * s_tLCDTextControl.ptFont->tCharSize.iHeight 
            >= s_tLCDTextControl.tRegion.tSize.iHeight) {
            s_tLCDTextControl.tTextLocation.chY = 0;
        }
    }
}

arm_2d_err_t arm_lcd_text_set_font(const arm_2d_font_t *ptFont)
{
    if (NULL == ptFont) {
        ptFont = &ARM_2D_FONT_6x8.use_as__arm_2d_font_t;   /* use default font */
    }

    if (s_tLCDTextControl.ptFont == ptFont) {
        return ARM_2D_ERR_NONE;
    }

    do {
        if (NULL == ptFont->tileFont.pchBuffer) {
            break;
        }
        if (    (0 == ptFont->tCharSize.iHeight) 
            ||  (0 == ptFont->tCharSize.iWidth)
            ||  (0 == ptFont->nCount)) {
            break;
        }
        
        s_tLCDTextControl.ptFont = ptFont;

        /* reset draw pointer */
        arm_lcd_text_location(0,0);
        
        return ARM_2D_ERR_NONE;
    } while(0);
    
    return ARM_2D_ERR_INVALID_PARAM;
}


arm_2d_char_descriptor_t *
ARM_2D_A1_FONT_GET_CHAR_DESCRIPTOR_HANDLER(
                                        const arm_2d_font_t *ptFont, 
                                        arm_2d_char_descriptor_t *ptDescriptor,
                                        uint8_t *pchCharCode)
{
    assert(NULL != ptFont);
    assert(NULL != ptDescriptor);
    assert(NULL != pchCharCode);
    
    arm_2d_a1_font_t *ptThis = (arm_2d_a1_font_t *)ptFont;
    
    memset(ptDescriptor, 0, sizeof(arm_2d_char_descriptor_t));
    
    ptDescriptor->tileChar.tRegion.tSize = ptFont->tCharSize;
    ptDescriptor->tileChar.ptParent = (arm_2d_tile_t *)&ptFont->tileFont;
    ptDescriptor->tileChar.tInfo.bDerivedResource = true;
    
    ptDescriptor->tileChar.tRegion.tLocation.iY = 
        (*pchCharCode - (int16_t)this.nOffset) * ptFont->tCharSize.iHeight;
    
    ptDescriptor->chCodeLength = 1;

    return ptDescriptor;
}


int8_t lcd_draw_char(int16_t iX, int16_t iY, uint8_t **ppchCharCode)
{
    assert(NULL != ppchCharCode);
    assert(NULL != (*ppchCharCode));

    ARM_2D_INVOKE(s_tLCDTextControl.ptFont->fnGetCharDescriptor,
        ARM_2D_PARAM(   s_tLCDTextControl.ptFont,
                        &s_tLCDTextControl.tCharDescriptor,
                        (uint8_t *)(*ppchCharCode)));

    (*ppchCharCode) += s_tLCDTextControl.tCharDescriptor.chCodeLength;

    arm_2d_region_t tDrawRegion = {
        .tLocation = {
            .iX = iX + s_tLCDTextControl.tCharDescriptor.BearingX,
            .iY = iY + s_tLCDTextControl.tCharDescriptor.BearingY,
         },
        .tSize = s_tLCDTextControl.tCharDescriptor.tileChar.tRegion.tSize,
    };

    if (NULL != s_tLCDTextControl.ptFont->fnDrawChar) {
        s_tLCDTextControl.ptFont->fnDrawChar(   
                                    s_tLCDTextControl.ptTargetFB,
                                    &tDrawRegion,
                                    &s_tLCDTextControl.tCharDescriptor.tileChar,
                                    s_tLCDTextControl.tColour.tForeground);
    } else {
        arm_2d_draw_pattern(&s_tLCDTextControl.tCharDescriptor.tileChar, 
                            s_tLCDTextControl.ptTargetFB, 
                            &tDrawRegion,
                            s_tLCDTextControl.wMode,
                            s_tLCDTextControl.tColour.tForeground,
                            s_tLCDTextControl.tColour.tBackground);
    }

    arm_2d_op_wait_async(NULL);

    return s_tLCDTextControl.tCharDescriptor.chKerning;
}

void arm_lcd_puts(const char *str)
{
    while(*str) {
        if (*str == '\r') {
            s_tLCDTextControl.tTextLocation.chX = 0;
        } else if (*str == '\n') {
            s_tLCDTextControl.tTextLocation.chX = 0;
            s_tLCDTextControl.tTextLocation.chY++;
        } else if (*str == '\t') { 
            s_tLCDTextControl.tTextLocation.chX += 8;
            s_tLCDTextControl.tTextLocation.chX &= ~(_BV(3)-1);

            if (    s_tLCDTextControl.tTextLocation.chX * s_tLCDTextControl.ptFont->tCharSize.iWidth 
                >=  s_tLCDTextControl.tRegion.tSize.iWidth ) {
                s_tLCDTextControl.tTextLocation.chX = 0;
                s_tLCDTextControl.tTextLocation.chY++;
            }
        }else if (*str == '\b') {
            if (s_tLCDTextControl.tTextLocation.chX) {
                s_tLCDTextControl.tTextLocation.chX--;
            }
        } else {

            int16_t iX = s_tLCDTextControl.tTextLocation.chX * s_tLCDTextControl.ptFont->tCharSize.iWidth;
            int16_t iY = s_tLCDTextControl.tTextLocation.chY * s_tLCDTextControl.ptFont->tCharSize.iHeight;

            lcd_draw_char(s_tLCDTextControl.tRegion.tLocation.iX + iX, 
                            s_tLCDTextControl.tRegion.tLocation.iY + iY, 
                            (uint8_t **)&str);

            s_tLCDTextControl.tTextLocation.chX++;
            if (    s_tLCDTextControl.tTextLocation.chX * s_tLCDTextControl.ptFont->tCharSize.iWidth 
                >=  s_tLCDTextControl.tRegion.tSize.iWidth ) {
                s_tLCDTextControl.tTextLocation.chX = 0;
                s_tLCDTextControl.tTextLocation.chY++;
                if (    s_tLCDTextControl.tTextLocation.chY * s_tLCDTextControl.ptFont->tCharSize.iHeight 
                    >= s_tLCDTextControl.tRegion.tSize.iHeight) {
                    s_tLCDTextControl.tTextLocation.chY = 0;
                }
            }

            continue;
        }
        
        str++;
    }
}

#if defined(__IS_COMPILER_IAR__) && __IS_COMPILER_IAR__
#define __va_list    va_list

#endif

int arm_lcd_printf(const char *format, ...)
{
    int real_size;
    static char s_chBuffer[__LCD_PRINTF_CFG_TEXT_BUFFER_SIZE__ + 1];
    __va_list ap;
    va_start(ap, format);
        real_size = vsnprintf(s_chBuffer, sizeof(s_chBuffer)-1, format, ap);
    va_end(ap);
    real_size = MIN(sizeof(s_chBuffer)-1, real_size);
    s_chBuffer[real_size] = '\0';
    arm_lcd_puts(s_chBuffer);
    return real_size;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
