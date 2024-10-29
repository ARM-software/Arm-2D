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

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        #include "arm_2d_helper_font.c"
 * Description:  the font helper service source code
 *
 * $Date:        29. Oct 2024
 * $Revision:    V.2.7.3
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/

#define __ARM_2D_IMPL__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "arm_2d.h"
#include "__arm_2d_impl.h"

#include "arm_2d_helper.h"

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
#elif __IS_COMPILER_ARM_COMPILER_5__
#   pragma diag_suppress 174,177,188,68,513,144,64
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

    arm_2d_scratch_mem_t tCharBuffer;
    arm_2d_region_t tTextRegion;

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


ARM_NONNULL(1)
int8_t arm_2d_helper_get_utf8_byte_valid_length(const uint8_t *pchChar)
{

    switch(__CLZ( ~((uint32_t)pchChar[0] << 24) )) {
        case 0:                                     /* BYTE0: 0xxx-xxxx */
            return 1;
        case 1:
            break;
        case 2:                                     /* BYTE0: 110x-xxxx */
            if ((pchChar[1] & 0xC0) == 0x80) {      /* BYTE1: 10xx-xxxx */
                return 2;
            }
            break;
        case 3:                                     /* BYTE0: 1110-xxxx */
            if  (((pchChar[1] & 0xC0) == 0x80)      /* BYTE1: 10xx-xxxx */
            &&   ((pchChar[2] & 0xC0) == 0x80)) {   /* BYTE2: 10xx-xxxx */
                return 3;
            }
            break;
        case 4:
            if  (((pchChar[1] & 0xC0) == 0x80)      /* BYTE1: 10xx-xxxx */
            &&   ((pchChar[2] & 0xC0) == 0x80)      /* BYTE2: 10xx-xxxx */
            &&   ((pchChar[3] & 0xC0) == 0x80)) {   /* BYTE3: 10xx-xxxx */
                return 4;
            }
            break;
        default:
            break;
    }

    return -1;
}

#define __UTF8_TO_UNICODE1(__B0)           ((uint16_t)(__B0))

#define __UTF8_TO_UNICODE2(__B0, __B1)                                          \
            (   ((uint16_t)(((__B0) & 0x1F) << 6))                              \
            |   ((uint16_t)((__B1) & 0x3F)))

#define __UTF8_TO_UNICODE3(__B0, __B1, __B2)                                    \
            (   ((uint16_t)(((__B0) & 0x0F) << 12))                             \
            |   ((uint16_t)(((__B1) & 0x3F) << 6))                              \
            |   ((uint16_t)((__B2) & 0x3F)))

#define __UTF8_TO_UNICODE4(__B0, __B1, __B2, __B3)                              \
            (   ((uint32_t)(((__B0) & 0x07) << 18))                             \
            |   ((uint32_t)(((__B1) & 0x3F) << 12))                             \
            |   ((uint32_t)(((__B2) & 0x3F) << 6))                              \
            |   ((uint32_t)((__B3) & 0x3F)))

#define __UTF8_TO_UNICODE(...)                                                  \
            ARM_CONNECT2(__UTF8_TO_UNICODE,                                     \
                         __ARM_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

ARM_NONNULL(1)
uint32_t arm_2d_helper_utf8_to_unicode(const uint8_t *pchUTF8)
{
    uint32_t wUTF16 = 0;

    switch(arm_2d_helper_get_utf8_byte_valid_length(pchUTF8)) {
        default:
        case 0:
            break;
        case 1:
            wUTF16 = __UTF8_TO_UNICODE(pchUTF8[0]);
            break;
        case 2:
            wUTF16 = __UTF8_TO_UNICODE(pchUTF8[0], pchUTF8[1]);
            break;
        case 3:
            wUTF16 = __UTF8_TO_UNICODE(pchUTF8[0], pchUTF8[1], pchUTF8[2]);
            break;
        case 4:
            wUTF16 = __UTF8_TO_UNICODE(pchUTF8[0], pchUTF8[1], pchUTF8[2], pchUTF8[3]);
            break;
    }

    return wUTF16;
}

ARM_NONNULL(1)
int8_t arm_2d_helper_get_utf8_byte_length(const uint8_t *pchChar)
{
    switch(__CLZ( ~((uint32_t)pchChar[0] << 24) )) {
        case 0:
            return 1;
        case 1:
            break;
        case 2:
            return 2;
        case 3:
            return 3;
        case 4:
            return 4;
        default:
            break;
    }

    return -1;
}

ARM_NONNULL(1,2,3)
arm_2d_char_descriptor_t *
arm_2d_helper_get_char_descriptor(  const arm_2d_font_t *ptFont, 
                                    arm_2d_char_descriptor_t *ptDescriptor, 
                                    uint8_t *pchCharCode)
{
    assert(NULL != pchCharCode);

    return ARM_2D_INVOKE(ptFont->fnGetCharDescriptor,
                ARM_2D_PARAM(   ptFont,
                                ptDescriptor,
                                pchCharCode));
}


void arm_lcd_text_init(arm_2d_region_t *ptScreen)
{
    assert(NULL != ptScreen);
    s_tLCDTextControl.tScreen = *ptScreen;
}

void arm_lcd_text_reset_display_region_tracking(void)
{
    memset(&s_tLCDTextControl.tTextRegion, 0, sizeof(arm_2d_region_t));
}

arm_2d_region_t *arm_lcd_text_get_last_display_region(void)
{
    return &s_tLCDTextControl.tTextRegion;
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

static void __arm_lcd_text_update_char_buffer(void)
{
    const arm_2d_font_t *ptFont = s_tLCDTextControl.ptFont;
    if (ARM_2D_COLOUR_MASK_A8 == ptFont->tileFont.tInfo.tColourInfo.chScheme) {
        /* free memory */
        arm_2d_scratch_memory_free(&s_tLCDTextControl.tCharBuffer);
        return ;
    }

    
    size_t tBufferSize = ptFont->tCharSize.iHeight * ptFont->tCharSize.iWidth;

    if (NULL != (void *)(s_tLCDTextControl.tCharBuffer.pBuffer)) {
        if (s_tLCDTextControl.tCharBuffer.u24SizeInByte >= tBufferSize) {
            return ;
        }

        arm_2d_scratch_memory_free(&s_tLCDTextControl.tCharBuffer);
    }

    arm_2d_scratch_memory_new(&s_tLCDTextControl.tCharBuffer,
                              1,
                              tBufferSize,
                              4,
                              ARM_2D_MEM_TYPE_FAST);
}

void arm_lcd_text_set_scale(float fScale)
{
    if ((fScale != 0.0f) && ABS(fScale - 1.0f) > 0.01f) {
        s_tLCDTextControl.fScale = ABS(fScale);

        __arm_lcd_text_update_char_buffer();
    } else {
        s_tLCDTextControl.fScale = 0.0f;
    }
}

bool arm_lcd_text_force_char_use_same_width(bool bForced)
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

static
int16_t __arm_lcd_get_char_advance(const arm_2d_font_t *ptFont, arm_2d_char_descriptor_t *ptDescriptor, uint8_t *pchChar)
{
    int16_t iAdvance = ptFont->tCharSize.iWidth;

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
        ptDescriptor = arm_2d_helper_get_char_descriptor(  ptFont, 
                                                        &tDescriptor,
                                                        pchChar);
        if (NULL == ptDescriptor){
            break;
        }
        iAdvance = ptDescriptor->iAdvance;
    } while(0);

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
    const arm_2d_font_t *ptFont = s_tLCDTextControl.ptFont;

    int8_t chCodeLength = arm_2d_helper_get_utf8_byte_valid_length(*ppchCharCode);
    if (chCodeLength <= 0) {
        chCodeLength = 1;
    }

    if (NULL == arm_2d_helper_get_char_descriptor(  ptFont, 
                                                    &tCharDescriptor,
                                                    *ppchCharCode)){
        (*ppchCharCode) += chCodeLength;

        return __arm_lcd_get_char_advance(ptFont, NULL, NULL);
    }

    //(*ppchCharCode) += tCharDescriptor.chCodeLength;
    (*ppchCharCode) += chCodeLength;

    arm_2d_size_t tBBoxSize = ptFont->tCharSize;

    arm_2d_region_t tDrawRegion = {
        .tLocation = {
            .iX = iX + tCharDescriptor.iBearingX,
            .iY = iY + (tBBoxSize.iHeight - tCharDescriptor.iBearingY),
         },
        .tSize = tCharDescriptor.tileChar.tRegion.tSize,
    };

    if (    (s_tLCDTextControl.tTextRegion.tSize.iHeight > 0)
       &&   (s_tLCDTextControl.tTextRegion.tSize.iWidth > 0)) {
        arm_2d_region_get_minimal_enclosure(&s_tLCDTextControl.tTextRegion,
                                            &tDrawRegion,
                                            &s_tLCDTextControl.tTextRegion);
    } else {
        s_tLCDTextControl.tTextRegion = tDrawRegion;
    }

    if (    (ptFont->tileFont.tColourInfo.chScheme == ARM_2D_COLOUR_BIN)
        &&  (ARM_2D_DRW_PATN_MODE_COPY != s_tLCDTextControl.wMode)
        &&  (ptFont->tileFont.tInfo.bHasEnforcedColour)) {
        arm_2d_draw_pattern(&tCharDescriptor.tileChar, 
                            s_tLCDTextControl.ptTargetFB, 
                            &tDrawRegion,
                            s_tLCDTextControl.wMode,
                            s_tLCDTextControl.tColour.tForeground,
                            s_tLCDTextControl.tColour.tBackground);
    } else if (NULL != ptFont->fnDrawChar) {
        ptFont->fnDrawChar( s_tLCDTextControl.ptTargetFB,
                            &tDrawRegion,
                            ptFont,
                            &tCharDescriptor.tileChar,
                            s_tLCDTextControl.tColour.tForeground,
                            chOpacity,
                            s_tLCDTextControl.fScale);

        if (s_tLCDTextControl.fScale > 0.0f) {
            ARM_2D_IMPL(arm_2d_op_fill_cl_msk_opa_trans_t, NULL);

            arm_2d_region_get_minimal_enclosure(&s_tLCDTextControl.tTextRegion,
                                                this.Target.ptRegion,
                                                &s_tLCDTextControl.tTextRegion);
        }
    } 

    ARM_2D_OP_WAIT_ASYNC();

    return __arm_lcd_get_char_advance(s_tLCDTextControl.ptFont, &tCharDescriptor, NULL);
}

IMPL_FONT_DRAW_CHAR(__arm_2d_lcd_text_default_a8_font_draw_char)
{
    ARM_2D_UNUSED(ptFont);

    if (fScale == 0.0f) {
        return arm_2d_fill_colour_with_mask_and_opacity(
                                            ptTile,
                                            ptRegion,
                                            ptileChar,
                                            (__arm_2d_color_t){tForeColour},
                                            chOpacity);
    } else {
        arm_2d_size_t tCharSize =  ptileChar->tRegion.tSize;
        arm_2d_location_t c_tCentre = {
            .iX = tCharSize.iWidth >> 1,
            .iY = tCharSize.iHeight >> 1,
        };

        arm_2d_location_t tTargetCenter = ptRegion->tLocation;
        tTargetCenter.iX += ptRegion->tSize.iWidth >> 1;
        tTargetCenter.iY += ptRegion->tSize.iHeight >> 1;

        return arm_2d_fill_colour_with_mask_opacity_and_transform(
                                                ptileChar,
                                                ptTile,
                                                NULL,
                                                c_tCentre,
                                                0.0f,
                                                fScale,
                                                tForeColour,
                                                chOpacity,
                                                &tTargetCenter);
    }
}

IMPL_FONT_DRAW_CHAR(__arm_2d_lcd_text_default_a1_font_draw_char)
{
#if __ARM_2D_CFG_SUPPORT_TRANSFORM_FOR_NON_A8_FONTS__
    if (fScale != 0.0f) {

        arm_2d_tile_t tTempA8CharTile = {
            .tRegion = {
                .tSize = ptFont->tCharSize,
            },
            .tInfo = {
                .bIsRoot = true,
                .bHasEnforcedColour = true,
                .tColourInfo = {
                    .chScheme = ARM_2D_COLOUR_8BIT,
                },
            },
            .nAddress = s_tLCDTextControl.tCharBuffer.pBuffer,
        };

        memset( (uint8_t *)tTempA8CharTile.nAddress, 
                0, 
                s_tLCDTextControl.tCharBuffer.u24SizeInByte);

    #if 0
        arm_2d_gray8_fill_colour_with_a1_mask(&tTempA8CharTile, NULL, ptileChar, 
                                             (arm_2d_color_gray8_t) {0xFF});
    #else
        do {
            __arm_2d_tile_param_t tSourceTileParam;
            uint_fast8_t chSourcePixelLenInBit;
            __arm_2d_tile_region_caculator( 
                                ptileChar, 
                                &tSourceTileParam,
                                &chSourcePixelLenInBit,
                                true,
                                0,
                                false); 

            __arm_2d_impl_c8bit_draw_pattern_fg_only(
                (uint8_t  *)tSourceTileParam.pBuffer,
                tSourceTileParam.nOffset,
                tSourceTileParam.iStride,
                (uint8_t  *)tTempA8CharTile.pchBuffer,
                ptFont->tCharSize.iWidth,
                &tSourceTileParam.tValidRegion.tSize,
                0xFF);

        } while(0);
    #endif

        arm_2d_region_t tCharRegion = {
            .tSize = ptileChar->tRegion.tSize,
        };
        arm_2d_tile_t tTempCharTile;
        arm_2d_tile_generate_child(&tTempA8CharTile, &tCharRegion, &tTempCharTile, false);

        return __arm_2d_lcd_text_default_a8_font_draw_char( ptTile,
                                                            ptRegion,
                                                            ptFont,
                                                            &tTempCharTile,
                                                            tForeColour,
                                                            chOpacity,
                                                            fScale);

    } else 
#endif
    {
        return arm_2d_fill_colour_with_a1_mask_and_opacity(
                                                ptTile,
                                                ptRegion,
                                                ptileChar,
                                                (__arm_2d_color_t){tForeColour},
                                                chOpacity);
    }

}

IMPL_FONT_DRAW_CHAR(__arm_2d_lcd_text_default_a2_font_draw_char)
{
#if __ARM_2D_CFG_SUPPORT_TRANSFORM_FOR_NON_A8_FONTS__
    if (fScale != 0.0f) {

        arm_2d_tile_t tTempA8CharTile = {
            .tRegion = {
                .tSize = ptFont->tCharSize,
            },
            .tInfo = {
                .bIsRoot = true,
                .bHasEnforcedColour = true,
                .tColourInfo = {
                    .chScheme = ARM_2D_COLOUR_8BIT,
                },
            },
            .nAddress = s_tLCDTextControl.tCharBuffer.pBuffer,
        };

        memset( (uint8_t *)tTempA8CharTile.nAddress, 
                0, 
                s_tLCDTextControl.tCharBuffer.u24SizeInByte);

    #if 0
        arm_2d_gray8_fill_colour_with_a2_mask(&tTempA8CharTile, NULL, ptileChar, 
                                             (arm_2d_color_gray8_t) {0xFF});
    #else
        do {
            __arm_2d_tile_param_t tSourceTileParam;
            uint_fast8_t chSourcePixelLenInBit;
            __arm_2d_tile_region_caculator( 
                                ptileChar, 
                                &tSourceTileParam,
                                &chSourcePixelLenInBit,
                                true,
                                0,
                                false); 

            __arm_2d_impl_gray8_colour_filling_a2_mask(
                (uint8_t  *)tTempA8CharTile.pchBuffer,
                ptFont->tCharSize.iWidth,
                (uint8_t  *)tSourceTileParam.pBuffer,
                tSourceTileParam.iStride,
                tSourceTileParam.nOffset,
                &tSourceTileParam.tValidRegion.tSize,
                0xFF);

        } while(0);
    #endif

        arm_2d_region_t tCharRegion = {
            .tSize = ptileChar->tRegion.tSize,
        };
        arm_2d_tile_t tTempCharTile;
        arm_2d_tile_generate_child(&tTempA8CharTile, &tCharRegion, &tTempCharTile, false);

        return __arm_2d_lcd_text_default_a8_font_draw_char( ptTile,
                                                            ptRegion,
                                                            ptFont,
                                                            &tTempCharTile,
                                                            tForeColour,
                                                            chOpacity,
                                                            fScale);

    } else 
#endif
    {
        return arm_2d_fill_colour_with_a2_mask_and_opacity(
                                                ptTile,
                                                ptRegion,
                                                ptileChar,
                                                (__arm_2d_color_t){tForeColour},
                                                chOpacity);
    }

}

IMPL_FONT_DRAW_CHAR(__arm_2d_lcd_text_default_a4_font_draw_char)
{
#if __ARM_2D_CFG_SUPPORT_TRANSFORM_FOR_NON_A8_FONTS__
    if (fScale != 0.0f) {

        arm_2d_tile_t tTempA8CharTile = {
            .tRegion = {
                .tSize = ptFont->tCharSize,
            },
            .tInfo = {
                .bIsRoot = true,
                .bHasEnforcedColour = true,
                .tColourInfo = {
                    .chScheme = ARM_2D_COLOUR_8BIT,
                },
            },
            .nAddress = s_tLCDTextControl.tCharBuffer.pBuffer,
        };

        memset( (uint8_t *)tTempA8CharTile.nAddress, 
                0, 
                s_tLCDTextControl.tCharBuffer.u24SizeInByte);

    #if 0
        arm_2d_gray8_fill_colour_with_a4_mask(&tTempA8CharTile, NULL, ptileChar, 
                                             (arm_2d_color_gray8_t) {0xFF});
    #else
        do {
            __arm_2d_tile_param_t tSourceTileParam;
            uint_fast8_t chSourcePixelLenInBit;
            __arm_2d_tile_region_caculator( 
                                ptileChar, 
                                &tSourceTileParam,
                                &chSourcePixelLenInBit,
                                true,
                                0,
                                false); 

            __arm_2d_impl_gray8_colour_filling_a4_mask(
                (uint8_t  *)tTempA8CharTile.pchBuffer,
                ptFont->tCharSize.iWidth,
                (uint8_t  *)tSourceTileParam.pBuffer,
                tSourceTileParam.iStride,
                tSourceTileParam.nOffset,
                &tSourceTileParam.tValidRegion.tSize,
                0xFF);

        } while(0);
    #endif

        arm_2d_region_t tCharRegion = {
            .tSize = ptileChar->tRegion.tSize,
        };
        arm_2d_tile_t tTempCharTile;
        arm_2d_tile_generate_child(&tTempA8CharTile, &tCharRegion, &tTempCharTile, false);

        return __arm_2d_lcd_text_default_a8_font_draw_char( ptTile,
                                                            ptRegion,
                                                            ptFont,
                                                            &tTempCharTile,
                                                            tForeColour,
                                                            chOpacity,
                                                            fScale);

    } else 
#endif
    {
        return arm_2d_fill_colour_with_a4_mask_and_opacity(
                                                ptTile,
                                                ptRegion,
                                                ptileChar,
                                                (__arm_2d_color_t){tForeColour},
                                                chOpacity);
    }

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

    if (s_tLCDTextControl.fScale > 0.0f) {
        tCharSize.iHeight = (float)tCharSize.iHeight * s_tLCDTextControl.fScale + 2;
        tCharSize.iWidth = (float)tCharSize.iWidth * s_tLCDTextControl.fScale + 2;
    }

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

            arm_2d_char_descriptor_t tCharDescriptor, *ptDescriptor;
            ptDescriptor = arm_2d_helper_get_char_descriptor(ptFont, 
                                                             &tCharDescriptor,
                                                             (uint8_t *)str);

            if (NULL != ptDescriptor) {
                int16_t tCharNewHeight = (tCharSize.iHeight - tCharDescriptor.iBearingY) + tCharSize.iHeight;

                tDrawBox.tSize.iHeight = MAX(tDrawBox.tSize.iHeight, tCharNewHeight);
            }

            
            tDrawBox.tLocation.iX += __arm_lcd_get_char_advance(ptFont, ptDescriptor, (uint8_t *)str);
            tDrawBox.tSize.iWidth = MAX(tDrawBox.tSize.iWidth, tDrawBox.tLocation.iX);

            str += chCodeLength;
            continue;
        }
        
        str++;
    }

    return tDrawBox.tSize;
}

void arm_lcd_putchar(const char *str)
{

    if (!arm_2d_helper_pfb_is_region_active(s_tLCDTextControl.ptTargetFB, &s_tLCDTextControl.tRegion, true)) {
        return ;
    }

    arm_2d_size_t tCharSize = s_tLCDTextControl.ptFont->tCharSize;

    if (s_tLCDTextControl.fScale > 0.0f) {
        tCharSize.iHeight = (float)tCharSize.iHeight * s_tLCDTextControl.fScale + 2;
        tCharSize.iWidth = (float)tCharSize.iWidth * s_tLCDTextControl.fScale + 2;
    }

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
                                              % (tCharSize.iWidth * 4);

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
    
    if (!arm_2d_helper_pfb_is_region_active(s_tLCDTextControl.ptTargetFB, &s_tLCDTextControl.tRegion, true)) {
        return ;
    }

    arm_2d_size_t tCharSize = s_tLCDTextControl.ptFont->tCharSize;

    if (s_tLCDTextControl.fScale > 0.0f) {
        tCharSize.iHeight = (float)tCharSize.iHeight * s_tLCDTextControl.fScale + 2;
        tCharSize.iWidth = (float)tCharSize.iWidth * s_tLCDTextControl.fScale + 2;
    }

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

#if 0
typedef enum {
    ARM_2D_ALIGN_LEFT               = _BV(0),                                   /*!< align to left */
    ARM_2D_ALIGN_RIGHT              = _BV(1),                                   /*!< align to right */
    ARM_2D_ALIGN_TOP                = _BV(2),                                   /*!< align to top */
    ARM_2D_ALIGN_BOTTOM             = _BV(3),                                   /*!< align to bottom */
    
    ARM_2D_ALIGN_CENTRE             = 0,                                        /*!< align to centre */
    ARM_2D_ALIGN_CENTRE_ALIAS       = ARM_2D_ALIGN_LEFT                         /*!< align to centre */
                                    | ARM_2D_ALIGN_RIGHT
                                    | ARM_2D_ALIGN_TOP
                                    | ARM_2D_ALIGN_BOTTOM,

    ARM_2D_ALIGN_TOP_LEFT           = ARM_2D_ALIGN_TOP                          /*!< align to top left corner */
                                    | ARM_2D_ALIGN_LEFT,
    ARM_2D_ALIGN_TOP_RIGHT          = ARM_2D_ALIGN_TOP                          /*!< align to top right corner */
                                    | ARM_2D_ALIGN_RIGHT,
    ARM_2D_ALIGN_BOTTOM_LEFT        = ARM_2D_ALIGN_BOTTOM                       /*!< align to bottom left corner */
                                    | ARM_2D_ALIGN_LEFT,
    ARM_2D_ALIGN_BOTTOM_RIGHT       = ARM_2D_ALIGN_BOTTOM                       /*!< align to bottom right corner */
                                    | ARM_2D_ALIGN_RIGHT,
} arm_2d_align_t ;
#endif

ARM_NONNULL(1)
void arm_lcd_puts_label( const char *pchString,
                        arm_2d_align_t tAlignment)
{
    assert(NULL != pchString);

    arm_2d_size_t tLabelSize = arm_lcd_get_string_line_box(pchString);
    arm_2d_region_t tOriginalDrawRegion = s_tLCDTextControl.tRegion;

    arm_2d_region_t tLabelRegion = {
        .tLocation = tOriginalDrawRegion.tLocation,
        .tSize = tLabelSize,
    };

    switch (tAlignment & (ARM_2D_ALIGN_LEFT | ARM_2D_ALIGN_RIGHT)) {
        case ARM_2D_ALIGN_LEFT:
            break;
        case ARM_2D_ALIGN_RIGHT:
            tLabelRegion.tLocation.iX += tOriginalDrawRegion.tSize.iWidth - tLabelSize.iWidth;
            break;
        default:
            tLabelRegion.tLocation.iX += (tOriginalDrawRegion.tSize.iWidth - tLabelSize.iWidth) >> 1;
            break;
    }

    switch (tAlignment & (ARM_2D_ALIGN_TOP | ARM_2D_ALIGN_BOTTOM)) {
        case ARM_2D_ALIGN_TOP:
            break;
        case ARM_2D_ALIGN_BOTTOM:
            tLabelRegion.tLocation.iY += tOriginalDrawRegion.tSize.iHeight - tLabelSize.iHeight;
            break;
        default:
            tLabelRegion.tLocation.iY += (tOriginalDrawRegion.tSize.iHeight - tLabelSize.iHeight) >> 1;
            break;
    }

    arm_lcd_text_set_draw_region(&tLabelRegion);
    arm_lcd_puts(pchString);
    arm_lcd_text_set_draw_region(&tOriginalDrawRegion);
}

ARM_NONNULL(2)
int arm_lcd_printf_label(   arm_2d_align_t tAlignment, 
                            const char *format, 
                            ...)
{
    int real_size;
    static char s_chBuffer[__LCD_PRINTF_CFG_TEXT_BUFFER_SIZE__ + 1];
    __va_list ap;
    va_start(ap, format);
        real_size = vsnprintf(s_chBuffer, sizeof(s_chBuffer)-1, format, ap);
    va_end(ap);
    real_size = MIN(sizeof(s_chBuffer)-1, real_size);
    s_chBuffer[real_size] = '\0';

    arm_lcd_puts_label(s_chBuffer, tAlignment);

    return real_size;
}



#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
