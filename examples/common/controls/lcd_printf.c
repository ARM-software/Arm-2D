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
#   pragma clang diagnostic ignored "-Wunreachable-code"
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
    arm_2d_region_t tScreen;
    arm_2d_region_t tRegion;

    arm_2d_location_t   tDrawOffset;
    
    struct {
        COLOUR_INT_TYPE     tForeground;
        COLOUR_INT_TYPE     tBackground;
    } tColour;
    uint8_t chOpacity;

    arm_2d_tile_t *ptTargetFB;
    uint32_t       wMode;
    float          fScale;

    uint32_t  bForceAllCharUseSameWidth : 1;
    uint32_t                            : 31;

    const arm_2d_font_t *ptFont;
} s_tLCDTextControl = {
    .tScreen = {
        .tSize = {
            .iWidth = 320,
            .iHeight = 240,
        },
    },
    .tRegion = { 
        .tSize = {
            .iWidth = 320,
            .iHeight = 240,
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
    .chOpacity = 255,
};

/*============================ IMPLEMENTATION ================================*/

void arm_lcd_text_init(arm_2d_region_t *ptScreen)
{
    assert(NULL != ptScreen);
    s_tLCDTextControl.tScreen = *ptScreen;
}


void arm_lcd_text_set_colour(COLOUR_INT_TYPE wForeground, COLOUR_INT_TYPE wBackground)
{
    s_tLCDTextControl.tColour.tForeground = wForeground;
    s_tLCDTextControl.tColour.tBackground = wBackground;
}

void arm_lcd_text_set_target_framebuffer(const arm_2d_tile_t *ptFrameBuffer)
{
    s_tLCDTextControl.ptTargetFB = (arm_2d_tile_t *)ptFrameBuffer;

    if (NULL == ptFrameBuffer) {
        // use default framebuffer
        s_tLCDTextControl.ptTargetFB = (arm_2d_tile_t *)(-1);
    }
}

void arm_lcd_text_set_display_mode(uint32_t wMode)
{
    s_tLCDTextControl.wMode = wMode;
}

void arm_lcd_text_set_opacity(uint8_t chOpacity)
{
    s_tLCDTextControl.chOpacity = chOpacity;
}

void arm_lcd_text_set_scale(float fScale)
{
    if ((fScale != 0.0f) && ABS(fScale - 1.0f) > 0.01f) {
        s_tLCDTextControl.fScale = ABS(fScale);
    } else {
        s_tLCDTextControl.fScale = 0.0f;
    }
}

bool arm_lcd_text_force_char_use_same_with(bool bForced)
{
    bool bOriginalValue = s_tLCDTextControl.bForceAllCharUseSameWidth;

    s_tLCDTextControl.bForceAllCharUseSameWidth = bForced;

    return bOriginalValue;
}

void arm_lcd_text_set_draw_region(arm_2d_region_t *ptRegion)
{
    if (NULL == ptRegion) {
        ptRegion =  &s_tLCDTextControl.tScreen;
    }
    
    s_tLCDTextControl.tRegion = *ptRegion;
    
    /* reset draw pointer */
    arm_lcd_text_location(0,0);
}

void arm_lcd_text_location(uint8_t chY, uint8_t chX)
{
    arm_2d_size_t tSize = s_tLCDTextControl.ptFont->tCharSize;
    
    s_tLCDTextControl.tDrawOffset.iX 
        = tSize.iWidth * chX % s_tLCDTextControl.tRegion.tSize.iWidth;
    s_tLCDTextControl.tDrawOffset.iY 
        = tSize.iHeight * 
        (   chY + tSize.iWidth * chX 
        /   s_tLCDTextControl.tRegion.tSize.iWidth);

    s_tLCDTextControl.tDrawOffset.iY %= s_tLCDTextControl.tRegion.tSize.iHeight;
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
        /* reset zoom */
        arm_lcd_text_set_scale(0.0f);

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
    
    ptDescriptor->iAdvance = ptFont->tCharSize.iWidth;
    ptDescriptor->iBearingX = 0;
    ptDescriptor->iBearingY = ptFont->tCharSize.iHeight;
    
    ptDescriptor->tileChar.tRegion.tLocation.iY = 
        (*pchCharCode - (int16_t)this.nOffset) * ptFont->tCharSize.iHeight;
    
    ptDescriptor->chCodeLength = 1;

    return ptDescriptor;
}

#if 0
static 
arm_2d_char_descriptor_t *__arm_lcd_get_char_descriptor(const arm_2d_font_t *ptFont, 
                                                        arm_2d_char_descriptor_t *ptDescriptor, 
                                                        uint8_t *pchCharCode)
{
    assert(NULL != pchCharCode);

    if (NULL == arm_2d_helper_get_char_descriptor(ptFont, ptDescriptor, pchCharCode)) {
        assert(false);
        return NULL;
    }

    /* NOTE: when the FONT mask insn't ARM_2D_COLOUR_8BIT, the scaling behaviour is unpredicted */
    if (s_tLCDTextControl.fScale > 0.0f) {
        ptDescriptor->iAdvance = (int16_t)((float)ptDescriptor->iAdvance * s_tLCDTextControl.fScale);
        /* NOTE: No need to adjust bearings in the following way. */
        //ptDescriptor->iBearingX = (int16_t)((float)ptDescriptor->iBearingX * s_tLCDTextControl.fScale);
        //ptDescriptor->iBearingY = (int16_t)((float)ptDescriptor->iBearingY * s_tLCDTextControl.fScale);
    }

    return ptDescriptor;
}
#endif

static
int16_t __arm_lcd_get_char_advance(arm_2d_char_descriptor_t *ptDescriptor, uint8_t *pchChar)
{
    int16_t iAdvance = s_tLCDTextControl.ptFont->tCharSize.iWidth;

    do {
        if (s_tLCDTextControl.bForceAllCharUseSameWidth) {
            break;
        }
        if (NULL != ptDescriptor) {
            iAdvance = ptDescriptor->iAdvance;
            break;
        } if (NULL == pchChar) {
            break;
        }

        arm_2d_char_descriptor_t tDescriptor;
        if (NULL == arm_2d_helper_get_char_descriptor(  s_tLCDTextControl.ptFont, 
                                                        &tDescriptor,
                                                        pchChar)){
            break;
        }
        iAdvance = ptDescriptor->iAdvance;
    } while(0);


    /* NOTE: when the FONT mask insn't ARM_2D_COLOUR_8BIT, the scaling behaviour is unpredicted */
    if (s_tLCDTextControl.fScale > 0.0f) {
        iAdvance = (int16_t)((float)iAdvance * s_tLCDTextControl.fScale);
        /* NOTE: No need to adjust bearings in the following way. */
        //ptDescriptor->iBearingX = (int16_t)((float)ptDescriptor->iBearingX * s_tLCDTextControl.fScale);
        //ptDescriptor->iBearingY = (int16_t)((float)ptDescriptor->iBearingY * s_tLCDTextControl.fScale);
    }

    return iAdvance;

}

int16_t lcd_draw_char(int16_t iX, int16_t iY, uint8_t **ppchCharCode, uint_fast8_t chOpacity)
{
    arm_2d_char_descriptor_t tCharDescriptor;
    
    int8_t chCodeLength = arm_2d_helper_get_utf8_byte_valid_length(*ppchCharCode);
    if (chCodeLength <= 0) {
        chCodeLength = 1;
    }

    if (NULL == arm_2d_helper_get_char_descriptor(  s_tLCDTextControl.ptFont, 
                                                    &tCharDescriptor,
                                                    *ppchCharCode)){
        (*ppchCharCode) += chCodeLength;

        return __arm_lcd_get_char_advance(NULL, NULL);
    }

    //(*ppchCharCode) += tCharDescriptor.chCodeLength;
    (*ppchCharCode) += chCodeLength;

    arm_2d_size_t tBBoxSize = s_tLCDTextControl.ptFont->tCharSize;

    arm_2d_region_t tDrawRegion = {
        .tLocation = {
            .iX = iX + tCharDescriptor.iBearingX,
            .iY = iY + (tBBoxSize.iHeight - tCharDescriptor.iBearingY),
         },
        .tSize = tCharDescriptor.tileChar.tRegion.tSize,
    };

    if (NULL != s_tLCDTextControl.ptFont->fnDrawChar) {
        s_tLCDTextControl.ptFont->fnDrawChar(   
                                    s_tLCDTextControl.ptTargetFB,
                                    &tDrawRegion,
                                    &tCharDescriptor.tileChar,
                                    s_tLCDTextControl.tColour.tForeground,
                                    chOpacity,
                                    s_tLCDTextControl.fScale);
    } else {
        arm_2d_draw_pattern(&tCharDescriptor.tileChar, 
                            s_tLCDTextControl.ptTargetFB, 
                            &tDrawRegion,
                            s_tLCDTextControl.wMode,
                            s_tLCDTextControl.tColour.tForeground,
                            s_tLCDTextControl.tColour.tBackground);
    }

    arm_2d_op_wait_async(NULL);

    return __arm_lcd_get_char_advance(&tCharDescriptor, NULL);
}

static void __arm_lcd_draw_region_line_wrapping(arm_2d_size_t *ptCharSize, 
                                                arm_2d_size_t *ptDrawRegionSize)
{
    if (s_tLCDTextControl.tDrawOffset.iX >= ptDrawRegionSize->iWidth) {
        s_tLCDTextControl.tDrawOffset.iX = 0;
        s_tLCDTextControl.tDrawOffset.iY += ptCharSize->iHeight;

        if (s_tLCDTextControl.tDrawOffset.iY >= ptDrawRegionSize->iHeight) {
            s_tLCDTextControl.tDrawOffset.iY = 0;
        }
    }
}

arm_2d_size_t __arm_lcd_get_string_line_box(const char *str, const arm_2d_font_t *ptFont)
{
    if (NULL == ptFont) {
        ptFont = s_tLCDTextControl.ptFont;
    }
    arm_2d_size_t tCharSize = ptFont->tCharSize;
    arm_2d_region_t tDrawBox = {
        .tSize.iHeight = tCharSize.iHeight,
    };


    while(*str) {
        if (*str == '\r') {
            tDrawBox.tLocation.iX = 0;
        } else if (*str == '\n') {
            tDrawBox.tLocation.iX = 0;
            tDrawBox.tLocation.iY += tCharSize.iHeight;

            tDrawBox.tSize.iHeight = MAX(tDrawBox.tSize.iHeight, tDrawBox.tLocation.iY);
        } else if (*str == '\t') { 
            tDrawBox.tLocation.iX += tCharSize.iWidth * 4;
            tDrawBox.tLocation.iX -= tDrawBox.tLocation.iX 
                                   % tCharSize.iWidth;

            tDrawBox.tSize.iWidth = MAX(tDrawBox.tSize.iWidth, tDrawBox.tLocation.iX);

        }else if (*str == '\b') {
            if (tDrawBox.tLocation.iX >= tCharSize.iWidth) {
                tDrawBox.tLocation.iX -= tCharSize.iWidth;
            } else {
                tDrawBox.tLocation.iX = 0;
            }
        } else {

            int8_t chCodeLength = arm_2d_helper_get_utf8_byte_valid_length((uint8_t *)str);
            if (chCodeLength <= 0) {
                chCodeLength = 1;
            }
            str += chCodeLength;

            tDrawBox.tLocation.iX += __arm_lcd_get_char_advance(NULL, (uint8_t *)str);
            tDrawBox.tSize.iWidth = MAX(tDrawBox.tSize.iWidth, tDrawBox.tLocation.iX);

            continue;
        }
        
        str++;
    }

    return tDrawBox.tSize;
}

void arm_lcd_putchar(const char *str)
{
    arm_2d_size_t tCharSize = s_tLCDTextControl.ptFont->tCharSize;
    arm_2d_size_t tDrawRegionSize = s_tLCDTextControl.tRegion.tSize;

    if (*str) {
        if (*str == '\r') {
            s_tLCDTextControl.tDrawOffset.iX = 0;
        } else if (*str == '\n') {
            s_tLCDTextControl.tDrawOffset.iX = 0;
            s_tLCDTextControl.tDrawOffset.iY += tCharSize.iHeight;
            if (s_tLCDTextControl.tDrawOffset.iY >= tDrawRegionSize.iHeight) {
                s_tLCDTextControl.tDrawOffset.iY = 0;
            }
        } else if (*str == '\t') { 
            s_tLCDTextControl.tDrawOffset.iX += tCharSize.iWidth * 4;
            s_tLCDTextControl.tDrawOffset.iX -= s_tLCDTextControl.tDrawOffset.iX 
                                              % tCharSize.iWidth;

            __arm_lcd_draw_region_line_wrapping(&tCharSize, &tDrawRegionSize);

        }else if (*str == '\b') {
            if (s_tLCDTextControl.tDrawOffset.iX >= tCharSize.iWidth) {
                s_tLCDTextControl.tDrawOffset.iX -= tCharSize.iWidth;
            } else {
                s_tLCDTextControl.tDrawOffset.iX = 0;
            }
        } else {
            int16_t iX = s_tLCDTextControl.tDrawOffset.iX + s_tLCDTextControl.tRegion.tLocation.iX;
            int16_t iY = s_tLCDTextControl.tDrawOffset.iY + s_tLCDTextControl.tRegion.tLocation.iY; 

            s_tLCDTextControl.tDrawOffset.iX 
                += lcd_draw_char(   iX, iY, (uint8_t **)&str, s_tLCDTextControl.chOpacity);

            __arm_lcd_draw_region_line_wrapping(&tCharSize, &tDrawRegionSize);
        }
    }
}

void arm_lcd_puts(const char *str)
{
    arm_2d_size_t tCharSize = s_tLCDTextControl.ptFont->tCharSize;
    arm_2d_size_t tDrawRegionSize = s_tLCDTextControl.tRegion.tSize;

    while(*str) {
        if (*str == '\r') {
            s_tLCDTextControl.tDrawOffset.iX = 0;
        } else if (*str == '\n') {
            s_tLCDTextControl.tDrawOffset.iX = 0;
            s_tLCDTextControl.tDrawOffset.iY += tCharSize.iHeight;
            if (s_tLCDTextControl.tDrawOffset.iY >= tDrawRegionSize.iHeight) {
                s_tLCDTextControl.tDrawOffset.iY = 0;
            }
        } else if (*str == '\t') { 
            s_tLCDTextControl.tDrawOffset.iX += tCharSize.iWidth * 4;
            s_tLCDTextControl.tDrawOffset.iX -= s_tLCDTextControl.tDrawOffset.iX 
                                              % tCharSize.iWidth;

            __arm_lcd_draw_region_line_wrapping(&tCharSize, &tDrawRegionSize);

        }else if (*str == '\b') {
            if (s_tLCDTextControl.tDrawOffset.iX >= tCharSize.iWidth) {
                s_tLCDTextControl.tDrawOffset.iX -= tCharSize.iWidth;
            } else {
                s_tLCDTextControl.tDrawOffset.iX = 0;
            }
        } else {
            int16_t iX = s_tLCDTextControl.tDrawOffset.iX + s_tLCDTextControl.tRegion.tLocation.iX;
            int16_t iY = s_tLCDTextControl.tDrawOffset.iY + s_tLCDTextControl.tRegion.tLocation.iY; 

            s_tLCDTextControl.tDrawOffset.iX 
                += lcd_draw_char(   iX, iY, (uint8_t **)&str, s_tLCDTextControl.chOpacity);

            __arm_lcd_draw_region_line_wrapping(&tCharSize, &tDrawRegionSize);

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
