/*
 * Copyright (c) 2009-2024 Arm Limited. All rights reserved.
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
#define __TEXT_BOX_IMPLEMENT__

#include "./arm_2d_example_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "text_box.h"
#include <assert.h>
#include <string.h>

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Wreserved-identifier"
#   pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wpadded"
#   pragma clang diagnostic ignored "-Wcast-qual"
#   pragma clang diagnostic ignored "-Wcast-align"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wunused-const-variable"
#   pragma clang diagnostic ignored "-Wmissing-declarations"
#   pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#endif

/*============================ MACROS ========================================*/

#if __GLCD_CFG_COLOUR_DEPTH__ == 8


#elif __GLCD_CFG_COLOUR_DEPTH__ == 16


#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#else
#   error Unsupported colour depth!
#endif

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef enum {
    TEXT_BOX_CHAR_NORMAL,
    TEXT_BOX_CHAR_SEPERATOR,
    TEXT_BOX_CHAR_END_OF_LINE,
    TEXT_BOX_CHAR_CUT_OFF,
} __text_box_char_type_t;

/*============================ PROTOTYPES ====================================*/
static
bool __text_box_get_and_analyze_one_line(text_box_t *ptThis,
                                         __text_box_line_info_t *ptLineInfo);

/*============================ LOCAL VARIABLES ===============================*/
static 
int32_t __c_string_io_read_char(text_box_t *ptThis, 
                                uintptr_t pObj, 
                                uint8_t *pchBuffer,
                                uint_fast16_t hwSize);

static 
int32_t __c_string_io_seek( text_box_t *ptThis, 
                            uintptr_t pObj,
                            int32_t nOffset,
                            text_box_seek_whence_t enWhence);

static 
bool __c_string_io_eof( text_box_t *ptTextBox, 
                        uintptr_t pObj);

static
ARM_NONNULL(1)
bool __text_box_detect_brick(   uint8_t *pchPT, 
                                __text_box_char_type_t tType,
                                __text_box_line_info_t *ptLineInfo,
                                int32_t nPoistion,
                                int16_t iLineWidth,
                                int16_t iPureCharWidth);

static
ARM_NONNULL(1)
void __text_box_update(text_box_t *ptThis);
/*============================ GLOBAL VARIABLES ==============================*/
const text_box_io_handler_t TEXT_BOX_IO_C_STRING_READER = {
    .fnGetChar  = &__c_string_io_read_char,
    .fnSeek     = &__c_string_io_seek,
    .fnEOF      = &__c_string_io_eof,
};

/*============================ IMPLEMENTATION ================================*/

__STATIC_INLINE 
int32_t __text_box_get_current_position(text_box_t *ptThis)
{
    assert(NULL != ptThis);

    return 
    ARM_2D_INVOKE(this.tCFG.tStreamIO.ptIO->fnSeek,
        ARM_2D_PARAM(  ptThis, 
                       this.tCFG.tStreamIO.pTarget,
                       0,
                       TEXT_BOX_SEEK_CUR));
}

__STATIC_INLINE 
int32_t __text_box_set_current_position(text_box_t *ptThis, int32_t nPoistion)
{
    assert(NULL != ptThis);

    return 
    ARM_2D_INVOKE(this.tCFG.tStreamIO.ptIO->fnSeek,
        ARM_2D_PARAM(  ptThis, 
                       this.tCFG.tStreamIO.pTarget,
                       nPoistion,
                       TEXT_BOX_SEEK_SET));
}

__STATIC_INLINE 
int32_t __text_box_move_position(text_box_t *ptThis, int32_t nOffset)
{
    assert(NULL != ptThis);

    return 
    ARM_2D_INVOKE(this.tCFG.tStreamIO.ptIO->fnSeek,
        ARM_2D_PARAM(  ptThis, 
                       this.tCFG.tStreamIO.pTarget,
                       nOffset,
                       TEXT_BOX_SEEK_CUR));
}

__STATIC_INLINE
int32_t __text_box_read_bytes(  text_box_t *ptThis, 
                                uint8_t *pchBuffer, 
                                uint_fast16_t hwSize)
{
    assert(NULL != ptThis);
    assert(NULL != pchBuffer);

    return ARM_2D_INVOKE(this.tCFG.tStreamIO.ptIO->fnGetChar,
            ARM_2D_PARAM(   ptThis, 
                            this.tCFG.tStreamIO.pTarget,
                            pchBuffer,
                            hwSize));
}

__STATIC_INLINE
bool __text_box_is_eof(text_box_t *ptThis)
{
    assert(NULL != ptThis);

    return ARM_2D_INVOKE(this.tCFG.tStreamIO.ptIO->fnEOF,
            ARM_2D_PARAM(   ptThis, 
                            this.tCFG.tStreamIO.pTarget));
}

#if 0
static
ARM_NONNULL(1,2)
void __scratch_memory_free( text_box_t *ptThis,
                            __text_box_scratch_mem_t *ptItem)
{
    assert(NULL != ptThis);
    assert(NULL != ptItem);
    
    memset(ptItem, 0, sizeof(__text_box_scratch_mem_t));

    //arm_irq_safe {
        ARM_LIST_STACK_PUSH(this.ptFreeList, ptItem);
    //}
}

static 
ARM_NONNULL(1)
__text_box_scratch_mem_t * __scratch_memory_get(text_box_t *ptThis)
{
    assert(NULL != ptThis);

    __text_box_scratch_mem_t *ptItem = NULL;

    //arm_irq_safe {
        ARM_LIST_STACK_POP(this.ptFreeList, ptItem);
    //}

    return ptItem;
}

static
ARM_NONNULL(1,2)
void __line_cache_enter(text_box_t *ptThis,
                        __text_box_scratch_mem_t *ptItem)
{
    assert(NULL != ptThis);
    assert(NULL != ptItem);
    int32_t iTargetLineNumber = ptItem->nLineNo;
    bool bInserted = false;

    __text_box_scratch_mem_t **pptItem = &this.ptLineCache;

    while (NULL != *pptItem) {
        int32_t iItemLineNumber = (*pptItem)->nLineNo;

        if (iTargetLineNumber < iItemLineNumber) {

            /* insert to the current place */
            ptItem->ptNext = *pptItem;
            *pptItem = ptItem;

            bInserted = true;
            this.hwLineCacheCount++;
            break;
        } else if (iItemLineNumber == iTargetLineNumber) {

            /* replace the old item */
            __text_box_scratch_mem_t *ptOldItem = (*pptItem);

            ptItem->ptNext = ptOldItem->ptNext;
            (*pptItem) = ptItem;

            __scratch_memory_free(ptThis, ptOldItem);

            bInserted = true;
            break;
        }

        /* move to next item */
        pptItem = &((*pptItem)->ptNext);
    }

    if (!bInserted) {
        /* add to the end of the list  */
        *pptItem = ptItem;
        ptItem->ptNext = NULL;
        this.hwLineCacheCount++;
    }
}

static
ARM_NONNULL(1)
__text_box_scratch_mem_t * __line_cache_find(text_box_t *ptThis,
                                             int32_t iLineNumber)
{
    assert(NULL != ptThis);
   
    __text_box_scratch_mem_t *ptItem = this.ptLineCache;

    while (NULL != ptItem) {

        if (iLineNumber == ptItem->nLineNo) {
            ptItem->hwActive = MAX(this.tCFG.hwScratchMemoryCount, 4);
            return ptItem;
        }

        if (ptItem->hwActive) {
            ptItem->hwActive--;
        }

        ptItem = ptItem->ptNext;
    }

    return NULL;
}

static
ARM_NONNULL(1)
void __line_cache_free_old(text_box_t *ptThis)
{
    assert(NULL != ptThis);

    __text_box_scratch_mem_t **pptItem = &this.ptLineCache;

    while (NULL != *pptItem) {
    
        if (0 == (*pptItem)->hwActive) {
            __text_box_scratch_mem_t *ptOldItem = (*pptItem);

            /* remove this item */
            *pptItem = (*pptItem)->ptNext;

            __scratch_memory_free(ptThis, ptOldItem);

            return;
        }

        (*pptItem)->hwActive--;

        /* move to next item */
        pptItem = &((*pptItem)->ptNext);
    }
}

static
ARM_NONNULL(1)
void __line_cache_invalid_all(text_box_t *ptThis)
{

    assert(NULL != ptThis);
   
    __text_box_scratch_mem_t *ptItem = this.ptLineCache;

    while (NULL != ptItem) {
        __text_box_scratch_mem_t *ptNextItem = ptItem->ptNext;

        __scratch_memory_free(ptThis, ptItem);

        ptItem = ptNextItem;
    }

    this.ptLineCache = NULL;

}
#else
static void __line_cache_invalid_all(text_box_t *ptThis)
{
    ARM_2D_UNUSED(ptThis);
}
#endif

ARM_NONNULL(1,2)
void text_box_init( text_box_t *ptThis,
                    text_box_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);
    memset(ptThis, 0, sizeof(text_box_t));
    this.tCFG = *ptCFG;

    if (NULL == this.tCFG.ptFont) {
        this.tCFG.ptFont = (arm_2d_font_t *)&ARM_2D_FONT_6x8;
    }

#if 0
    if (    (NULL != ptCFG->ptScratchMemory)
       &&   (ptCFG->hwScratchMemoryCount > 0)) {

        uint_fast16_t hwCount = ptCFG->hwScratchMemoryCount;
        __text_box_scratch_mem_t *ptItem = ptCFG->ptScratchMemory;

        /* add scrach memory to pool */
        do {
            __scratch_memory_free(ptThis, ptItem++);
        } while(--hwCount);
    }
#endif

    if (ptCFG->bUseDirtyRegions && NULL != this.tCFG.ptScene) {



    }

}

ARM_NONNULL(1)
void text_box_on_load( text_box_t *ptThis)
{
    assert(NULL != ptThis);

    if (NULL != this.tCFG.ptScene) {
        arm_2d_helper_dirty_region_add_items(
            &this.tCFG.ptScene->tDirtyRegionHelper,
            this.tDirtyRegionItem,
            dimof(this.tDirtyRegionItem));
    }
}

ARM_NONNULL(1)
void text_box_depose( text_box_t *ptThis)
{
    assert(NULL != ptThis);

    if (    (this.tCFG.bUseDirtyRegions) 
        &&  (NULL != this.tCFG.ptScene)) {
        
        arm_2d_helper_dirty_region_remove_items(
                        &this.tCFG.ptScene->tDirtyRegionHelper,
                        this.tDirtyRegionItem,
                        dimof(this.tDirtyRegionItem));
    }
}

ARM_NONNULL(1)
void text_box_on_frame_start( text_box_t *ptThis)
{
    assert(NULL != ptThis);

}

ARM_NONNULL(1)
void text_box_on_frame_complete( text_box_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
int32_t text_box_set_start_line(text_box_t *ptThis, int32_t iStartLine)
{
    assert(NULL != ptThis);

    int32_t iOldStartLine = 0;
    
    arm_irq_safe {
        iOldStartLine = this.nTargetStartLineReq;

        if (iStartLine >= 0) {
            this.nTargetStartLineReq = iStartLine;
        }
    }

    return iOldStartLine;
}

ARM_NONNULL(1)
int32_t text_box_get_start_line(text_box_t *ptThis)
{
    return this.nTargetStartLineReq;
}

static
ARM_NONNULL(1,2)
void __text_box_draw_line(text_box_t *ptThis,
                          __text_box_line_info_t *ptLineInfo,
                          const arm_2d_tile_t *ptTile,
                          arm_2d_region_t *ptRegion,
                          text_box_line_alignment_t tAlign)
{
    assert(NULL != ptThis);
    assert(NULL != ptLineInfo);

    int32_t nPosition = ptLineInfo->nStartPosition;
    int32_t nBrickCount = ptLineInfo->hwBrickCount;
    int32_t nBlankCount = nBrickCount - 1;

    q16_t q16RisidualPixel = 0;

    /* nothing to draw */
    if (0 == nBrickCount || 0 == ptLineInfo->hwByteCount) {
        return ;
    }

    arm_2d_region_t tLineRegion = *ptRegion;

    /* handle draw region */
    if (TEXT_BOX_LINE_ALIGN_RIGHT == tAlign) {
        tLineRegion.tLocation.iX += this.iLineWidth - ptLineInfo->iLineWidth;
        tLineRegion.tSize.iWidth = ptLineInfo->iLineWidth;
    }

    arm_lcd_text_set_draw_region(&tLineRegion);

    //arm_2d_draw_box(ptTile, &tLineRegion, 1, GLCD_COLOR_BLUE, 200);
    __text_box_set_current_position(ptThis, nPosition);

    if (TEXT_BOX_LINE_ALIGN_JUSTIFIED == tAlign && !ptLineInfo->bEndNaturally) {
        uint16_t hwBytesLeft = ptLineInfo->hwByteCount;

        uint8_t chBrickDetectorPT = 0;

        do {
            bool bDetectBrick = false;
            union {
                uint32_t wValue;
                uint8_t chByte[4];
            } tUTF8Char = {0};

            int32_t nActualReadSize = __text_box_read_bytes(ptThis, tUTF8Char.chByte, 4);
            int_fast8_t chUTF8Length = arm_2d_helper_get_utf8_byte_length(tUTF8Char.chByte);
            nActualReadSize -= chUTF8Length;
            if (nActualReadSize > 0) {
                /* move back */
                __text_box_move_position(ptThis, -nActualReadSize);
            }
            hwBytesLeft -= chUTF8Length;

            bool bNormalChar = true;

            switch (tUTF8Char.chByte[0]) {
                case '\n':
                    bNormalChar = false;
                    bDetectBrick = __text_box_detect_brick(
                                                        &chBrickDetectorPT,
                                                        TEXT_BOX_CHAR_END_OF_LINE,
                                                        NULL,
                                                        0,
                                                        0,
                                                        0);
                    break;
                case '\t':
                    bNormalChar = false;
                    bDetectBrick = __text_box_detect_brick(
                                                        &chBrickDetectorPT,
                                                        TEXT_BOX_CHAR_SEPERATOR,
                                                        NULL,
                                                        0,
                                                        0,
                                                        0);
                    break;
                case ' ':
                    bNormalChar = false;
                    /* detect a blank space */
                    bDetectBrick = __text_box_detect_brick(
                                                        &chBrickDetectorPT,
                                                        TEXT_BOX_CHAR_SEPERATOR,
                                                        NULL,
                                                        0,
                                                        0,
                                                        0);
                    break;
                default:
                    if (tUTF8Char.chByte[0] < 0x20) {
                        /* ignore invisible chars */
                        bNormalChar = false;
                        bDetectBrick = __text_box_detect_brick(
                                                        &chBrickDetectorPT,
                                                        TEXT_BOX_CHAR_SEPERATOR,
                                                        NULL,
                                                        0,
                                                        0,
                                                        0);
                    }
                    break;
            }
    
            if (bNormalChar) {
                bDetectBrick = __text_box_detect_brick(
                                &chBrickDetectorPT,
                                TEXT_BOX_CHAR_NORMAL,
                                NULL,
                                0,
                                0,
                                0);

                if (!arm_lcd_putchar_to_buffer(tUTF8Char.chByte, chUTF8Length)) {
                    arm_lcd_printf_buffer(0);
                    arm_lcd_putchar_to_buffer(tUTF8Char.chByte, chUTF8Length);
                }
            }

            if (bDetectBrick) {
                if (nBlankCount) {
                    arm_lcd_printf_buffer(0);
                    nBlankCount--;
                    q16RisidualPixel += ptLineInfo->q16PixelsPerBlank;
                    int16_t iSpacing = reinterpret_s16_q16(q16RisidualPixel);
                    q16RisidualPixel = q16RisidualPixel - reinterpret_q16_f32(iSpacing);
                    arm_lcd_text_insert_line_space(iSpacing);
                }
            }
            
        } while(hwBytesLeft);
        arm_lcd_printf_buffer(0);

    } else {
        uint16_t hwBytesLeft = ptLineInfo->hwByteCount;

        do {
            union {
                uint32_t wValue;
                uint8_t chByte[4];
            } tUTF8Char = {0};

            int32_t nActualReadSize = __text_box_read_bytes(ptThis, tUTF8Char.chByte, 4);
            int_fast8_t chUTF8Length = arm_2d_helper_get_utf8_byte_length(tUTF8Char.chByte);
            nActualReadSize -= chUTF8Length;
            if (nActualReadSize > 0) {
                /* move back */
                __text_box_move_position(ptThis, -nActualReadSize);
            }
            hwBytesLeft -= chUTF8Length;

            if (!arm_lcd_putchar_to_buffer(tUTF8Char.chByte, chUTF8Length)) {
                arm_lcd_printf_buffer(0);
                arm_lcd_putchar_to_buffer(tUTF8Char.chByte, chUTF8Length);
            }
            
        } while(hwBytesLeft);
        arm_lcd_printf_buffer(0);
    }
}

ARM_NONNULL(1)
void text_box_show( text_box_t *ptThis,
                    const arm_2d_tile_t *ptTile, 
                    const arm_2d_region_t *ptRegion,
                    __arm_2d_color_t tColour,
                    uint8_t chOpacity,
                    bool bIsNewFrame)
{
    if (-1 == (intptr_t)ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
    }

    assert(NULL!= ptThis);

    arm_2d_container(ptTile, __text_box, ptRegion) {

        arm_lcd_text_set_target_framebuffer((arm_2d_tile_t *)&__text_box);
        arm_lcd_text_set_font((const arm_2d_font_t *)this.tCFG.ptFont);
        arm_lcd_text_set_colour(tColour.tValue, GLCD_COLOR_BLACK);
        arm_lcd_text_set_opacity(chOpacity);
        arm_lcd_text_set_scale(this.tCFG.fScale);

        if (bIsNewFrame) {
            bool bRequestUpdate = this.bUpdateReq;

            if (this.nTargetStartLineReq != this.Start.nLine) {
                bRequestUpdate = true;
            }

            if (this.iLineWidth != __text_box_canvas.tSize.iWidth) {
                this.iLineWidth = __text_box_canvas.tSize.iWidth 
                                - (arm_lcd_text_get_actual_char_size().iWidth >> 2);    //!< some chars advance might smaller than the char width, so we need this compenstation.
                bRequestUpdate = true;
            }

            if (bRequestUpdate) {
                this.bUpdateReq = false;
                __text_box_update(ptThis);

                if (this.tCFG.bUseDirtyRegions) {
                    arm_2d_helper_dirty_region_update_item( &this.tDirtyRegionItem[0],
                        (arm_2d_tile_t *)&__text_box,
                        NULL,
                        &__text_box_canvas);
                }
            }
        }

        /* move to the start */
        __text_box_set_current_position(ptThis, this.Start.nPosition);
        
        int32_t iLineNumber = this.Start.nLine;
        int16_t iFontCharBoxHeight = arm_lcd_text_get_actual_char_box().iHeight;
        int16_t iFontCharHeight = arm_lcd_text_get_actual_char_size().iHeight;
        /* tPFBScanRegion is a mapping of the PFB region inside the __text_box_canvas,
         * with which we can ignore the lines that out of the PFB region.
         */
        arm_2d_region_t tPFBScanRegion;
        do {
            arm_2d_region_t tValidRegion;
            if (!__arm_2d_tile_get_virtual_screen_or_root(  
                                                    &__text_box,
                                                    &tValidRegion, 
                                                    &tPFBScanRegion.tLocation,
                                                    NULL,
                                                    false)) {
                return ;
            }

            tPFBScanRegion.tSize = tValidRegion.tSize;
        } while(0);

        /* print all lines */
        int16_t iLineVerticalOffset = 0;
        do {
            #if 0
            if (__text_box_is_eof(ptThis)) {
                break;
            }
            #endif

            if (!__text_box_get_and_analyze_one_line(ptThis, &this.tCurrentLine)) {
                break;
            }

            int32_t iLineOffset = iLineNumber - this.Start.nLine;
            arm_2d_region_t tLineRegion = {
                .tLocation = {
                    .iY = iLineOffset * iFontCharBoxHeight + iLineVerticalOffset,
                },
                .tSize = {
                    .iHeight = iFontCharHeight,
                },
            };

            if (this.tCFG.u2LineAlign == TEXT_BOX_LINE_ALIGN_JUSTIFIED) {
                tLineRegion.tSize.iWidth = this.iLineWidth;
            } else {
                tLineRegion.tSize.iWidth = this.tCurrentLine.iLineWidth;
            }

            /* update line info */
            this.tCurrentLine.nLineNo = iLineNumber;

            bool bIgnoreDrawing = false;
            if (0 != tLineRegion.tSize.iWidth ) {
                if (!arm_2d_region_intersect(&__text_box_canvas, &tLineRegion, &tLineRegion)) {
                    /* out of text box canvas */
                    break;
                }

                arm_2d_region_t tExpandedLineRegion = tLineRegion;
                tExpandedLineRegion.tLocation.iY -= iFontCharHeight / 2;
                tExpandedLineRegion.tSize.iHeight = iFontCharHeight * 2;

                if (!arm_2d_region_intersect(&tPFBScanRegion, &tExpandedLineRegion, NULL)) {
                    /* out of canvas */
                    bIgnoreDrawing = true;

                    if (tLineRegion.tLocation.iY >= tPFBScanRegion.tLocation.iY + tPFBScanRegion.tSize.iHeight) {
                        /* the target line is below the PFB bottom line, no need to draw the following lines */
                        break;
                    }
                }
            }

            if (!bIgnoreDrawing) {
                __text_box_draw_line(ptThis,
                                     &this.tCurrentLine,
                                     &__text_box,
                                     &tLineRegion,
                                     this.tCFG.u2LineAlign);

            }

            int32_t nNewLinePosition = this.tCurrentLine.nStartPosition + this.tCurrentLine.hwByteCount;

            if (this.tCurrentLine.hwBrickCount > 0) {
                /* skip the `\n` in a paragraph */
                nNewLinePosition += !!this.tCurrentLine.bEndNaturally;
            }

            __text_box_set_current_position(ptThis, nNewLinePosition);
            
            if (this.tCurrentLine.bEndNaturally) {
                /* end of a paragraph, insert spacing */
                iLineVerticalOffset += this.tCFG.chSpaceBetweenParagraph;
            }

            iLineNumber++;
        } while(1);
    }

    ARM_2D_OP_WAIT_ASYNC();
}

/*!
 * \brief detect bricks and update line info
 * \return bool whether detect a brick
 */
static
ARM_NONNULL(1)
bool __text_box_detect_brick(   uint8_t *pchPT, 
                                __text_box_char_type_t tType,
                                __text_box_line_info_t *ptLineInfo,
                                int32_t nPoistion,
                                int16_t iLineWidth,
                                int16_t iPureCharWidth)
{
    assert(NULL != pchPT);

ARM_PT_BEGIN(*pchPT)

    /* wait for a normal char as the begin of tha brick */
    ARM_PT_ENTRY()
        if (tType != TEXT_BOX_CHAR_NORMAL) {

            if (TEXT_BOX_CHAR_CUT_OFF == tType || TEXT_BOX_CHAR_END_OF_LINE == tType) {
                if (0 == ptLineInfo->hwBrickCount) {
                    ptLineInfo->nStartPosition = nPoistion;
                    ptLineInfo->hwByteCount = 1;
                }

                ptLineInfo->bEndNaturally = (TEXT_BOX_CHAR_END_OF_LINE == tType);
            }

            ARM_PT_GOTO_PREV_ENTRY(false)
        }

        if (NULL != ptLineInfo) {
            if (0 == ptLineInfo->hwBrickCount) {
                ptLineInfo->nStartPosition = nPoistion;
            }
        }

    /* accept for normal chars */
    ARM_PT_ENTRY()
        if (tType == TEXT_BOX_CHAR_NORMAL) {
            ARM_PT_GOTO_PREV_ENTRY(false)
        }

        if (tType == TEXT_BOX_CHAR_CUT_OFF) {
            /* end a line and cut a brick off */

            if (NULL != ptLineInfo) {
                if (0 == ptLineInfo->hwBrickCount) {
                    /* the box is too small to contain one brick */
                    ptLineInfo->hwBrickCount++;
                    ptLineInfo->iLineWidth = iLineWidth;
                    ptLineInfo->iPureCharWidth = iPureCharWidth;
                    ptLineInfo->hwByteCount = nPoistion - ptLineInfo->nStartPosition;
                    ARM_PT_RETURN(true);
                }
            }

            ARM_PT_RETURN(false);
        }

        /* normal end of a brick */
        if (NULL != ptLineInfo) {
            ptLineInfo->hwBrickCount++;
            ptLineInfo->iLineWidth = iLineWidth;
            ptLineInfo->iPureCharWidth = iPureCharWidth;
            ptLineInfo->hwByteCount = nPoistion - ptLineInfo->nStartPosition;

            ptLineInfo->bEndNaturally = (TEXT_BOX_CHAR_END_OF_LINE == tType);
        }

ARM_PT_END()

    return true;
}



static
bool __text_box_get_and_analyze_one_line(text_box_t *ptThis,
                                         __text_box_line_info_t *ptLineInfo)
{
    assert(NULL != ptThis);
    assert(NULL != this.tCFG.tStreamIO.ptIO);
    assert(NULL != this.tCFG.tStreamIO.ptIO->fnGetChar);

    union {
        uint32_t wValue;
        uint8_t chByte[4];
    } tUTF8Char = {.chByte = {[0] = ' '}};

    if (NULL != ptLineInfo) {
        memset(ptLineInfo, 0, sizeof(__text_box_line_info_t));
    }

    uint8_t chBrickDetectorPT = 0;

    int16_t iLineWidth = 0;
    //int16_t iFontCharWidth = this.tCFG.ptFont->tCharSize.iWidth;
    bool bGetAValidLine = false;
    //arm_2d_char_descriptor_t tDescriptor;
    int16_t iWhiteSpaceWidth = arm_lcd_get_char_advance(tUTF8Char.chByte)
                             + arm_lcd_text_get_actual_spacing().iWidth;

    int16_t iPureCharWidth = 0;

    bool bDetectFirstBrick = false;
    do {
        bool bEndOfStream = false;
        tUTF8Char.wValue = 0;

        int32_t nPosition = __text_box_get_current_position(ptThis);
        int32_t nActualReadSize = __text_box_read_bytes(ptThis, 
                                                        tUTF8Char.chByte,
                                                        4);

        if (nActualReadSize <= 0) {
            /* we reach end of the stream */
            __text_box_detect_brick(&chBrickDetectorPT,
                                    TEXT_BOX_CHAR_END_OF_LINE,
                                    ptLineInfo,
                                    nPosition,
                                    iLineWidth,
                                    iPureCharWidth);
            break;
        }

        bGetAValidLine = true;

        nActualReadSize -= arm_2d_helper_get_utf8_byte_length(tUTF8Char.chByte);
        if (nActualReadSize > 0) {
            /* move back */
            __text_box_move_position(ptThis, -nActualReadSize);
        }

        bool bNormalChar = true;
        switch (tUTF8Char.chByte[0]) {
            case '\n':
                bNormalChar = false;
                bEndOfStream = true;
                __text_box_detect_brick(&chBrickDetectorPT,
                                        TEXT_BOX_CHAR_END_OF_LINE,
                                        ptLineInfo,
                                        nPosition,
                                        iLineWidth,
                                        iPureCharWidth);
                break;
            case '\t':
                bNormalChar = false;
                __text_box_detect_brick(&chBrickDetectorPT,
                                        TEXT_BOX_CHAR_SEPERATOR,
                                        ptLineInfo,
                                        nPosition,
                                        iLineWidth,
                                        iPureCharWidth);
                if (bDetectFirstBrick) {
                    iLineWidth += iWhiteSpaceWidth * 4;
                }
                break;
            case ' ':
                bNormalChar = false;
                /* detect a blank space */
                __text_box_detect_brick(&chBrickDetectorPT,
                                        TEXT_BOX_CHAR_SEPERATOR,
                                        ptLineInfo,
                                        nPosition,
                                        iLineWidth,
                                        iPureCharWidth);

                if (bDetectFirstBrick) {
                    iLineWidth += iWhiteSpaceWidth;
                }
                break;
            default:
                if (tUTF8Char.chByte[0] < 0x20) {
                    /* ignore invisible chars */
                    bNormalChar = false;
                    __text_box_detect_brick(&chBrickDetectorPT,
                                        TEXT_BOX_CHAR_SEPERATOR,
                                        ptLineInfo,
                                        nPosition,
                                        iLineWidth,
                                        iPureCharWidth);
                }
                break;
        }

        if (bNormalChar) {
            bDetectFirstBrick = true;

            int16_t iAdvance = arm_lcd_get_char_advance(tUTF8Char.chByte);
            int16_t iNewLineWidth = iLineWidth + iAdvance;

            if (iNewLineWidth > this.iLineWidth) {
                /* insufficient space for this char */
                bEndOfStream = true;
                __text_box_detect_brick(&chBrickDetectorPT,
                                        TEXT_BOX_CHAR_CUT_OFF,
                                        ptLineInfo,
                                        nPosition,
                                        iLineWidth,
                                        iPureCharWidth);
            } else {
                iPureCharWidth += iAdvance;

                iLineWidth = iNewLineWidth;
                __text_box_detect_brick(&chBrickDetectorPT,
                                        TEXT_BOX_CHAR_NORMAL,
                                        ptLineInfo,
                                        nPosition,
                                        iLineWidth,
                                        iPureCharWidth);
            }
        } else if (iLineWidth >= this.iLineWidth) {
            /* end of line, cut off */
            bEndOfStream = true;
            __text_box_detect_brick(&chBrickDetectorPT,
                                    TEXT_BOX_CHAR_CUT_OFF,
                                    ptLineInfo,
                                    nPosition,
                                    iLineWidth,
                                    iPureCharWidth);
        }

        if (bEndOfStream) {
            break;
        }

    } while(1);

    if (NULL != ptLineInfo) {
        do {
            if (ptLineInfo->hwBrickCount <= 1) {
                break;
            }

            int16_t iResidualWidth = 0;
            
            if (TEXT_BOX_LINE_ALIGN_JUSTIFIED == this.tCFG.u2LineAlign) {
                iResidualWidth = this.iLineWidth - ptLineInfo->iPureCharWidth;
            } else {
                iResidualWidth = this.iLineWidth - ptLineInfo->iLineWidth;
            }
            assert(iResidualWidth >= 0);

            if (iResidualWidth <= 0) {
                break;
            }

            /* calculate additional char space */
            ptLineInfo->q16PixelsPerBlank 
                = div_n_q16(reinterpret_q16_s16(iResidualWidth), 
                            (ptLineInfo->hwBrickCount - 1));        /* number of blanks */
        } while(0);
    }

    return bGetAValidLine;
}

static
ARM_NONNULL(1)
void __text_box_update(text_box_t *ptThis)
{
    assert(NULL != ptThis);
    assert(NULL != this.tCFG.tStreamIO.ptIO);
    assert(NULL != this.tCFG.tStreamIO.ptIO->fnSeek);

    int32_t nLineNumber = 0;

    bool bFastUpdate = false;
    if (this.nTargetStartLineReq != this.Start.nLine) {

        if (    (this.nTargetStartLineReq > this.Start.nLine)
           &&   (this.Start.nPosition > 0)) {
            bFastUpdate = true;

            nLineNumber = this.Start.nLine;
            __text_box_set_current_position(ptThis, this.Start.nPosition);
        }

        this.Start.nLine = this.nTargetStartLineReq;
    }

    if (!bFastUpdate) {
        /* move to the begin */
        __text_box_set_current_position(ptThis, 0);
        __line_cache_invalid_all(ptThis);
    } 

    __text_box_line_info_t tLineInfo;
    /* find the reading position of the start line */
    do {
        /* get the current position */
        if (this.Start.nLine == nLineNumber) {
            this.Start.nPosition = __text_box_get_current_position(ptThis);
            break;
        }

        if (!__text_box_get_and_analyze_one_line(ptThis, &tLineInfo)) {
            /* failed to read a line */
            break;
        }
        int32_t nNewLinePosition = tLineInfo.nStartPosition + tLineInfo.hwByteCount;
        if (tLineInfo.hwBrickCount > 0) {
            /* skip the `\n` in a paragraph */
            nNewLinePosition += !!tLineInfo.bEndNaturally;
        }
        __text_box_set_current_position(ptThis, nNewLinePosition);

        nLineNumber++;
    } while(1);

}

ARM_NONNULL(1)
void text_box_update(text_box_t *ptThis)
{
    assert(NULL != ptThis);

    arm_irq_safe {
        this.bUpdateReq = true;
    }
}

ARM_NONNULL(1,2)
text_box_c_str_reader_t *text_box_c_str_reader_init(
                                                text_box_c_str_reader_t *ptThis,
                                                const char *pchString,
                                                int32_t nMaxLen)
{
    assert(NULL != ptThis);
    assert(NULL != pchString);

    memset(ptThis, 0, sizeof(text_box_c_str_reader_t));
    this.pchString = pchString;
    this.nSizeInByte = strnlen(pchString, nMaxLen);

    return ptThis;
}

static 
int32_t __c_string_io_read_char(   text_box_t *ptTextBox, 
                                        uintptr_t pObj, 
                                        uint8_t *pchBuffer,
                                        uint_fast16_t hwSize)
{
    text_box_c_str_reader_t *ptThis = (text_box_c_str_reader_t *)pObj;
    assert(NULL != ptThis);
    
    if (NULL == pchBuffer) {
        return -1;
    } else if (0 == hwSize) {
        return 0;
    } else if (this.nPosition >= this.nSizeInByte) {
        return 0;
    }

    uintptr_t wReadPosition = this.nPosition + (uintptr_t)this.pchString;

    int32_t tLeftToRead = this.nSizeInByte - this.nPosition;
    hwSize = MIN(tLeftToRead, hwSize);

    memcpy(pchBuffer, (uint8_t *)wReadPosition, hwSize);
    this.nPosition += hwSize;

    return hwSize;
}

static 
int32_t __c_string_io_seek( text_box_t *ptTextBox, 
                            uintptr_t pObj,
                            int32_t nOffset,
                            text_box_seek_whence_t enWhence)
{
    text_box_c_str_reader_t *ptThis = (text_box_c_str_reader_t *)pObj;
    assert(NULL != ptThis);
    int64_t lPosition = this.nPosition;
    switch (enWhence) {
        case TEXT_BOX_SEEK_SET:
            lPosition = nOffset;
            break;
        case TEXT_BOX_SEEK_CUR:
            lPosition = this.nPosition + nOffset;
            break;
        case TEXT_BOX_SEEK_END:
            lPosition = (this.nSizeInByte - 1) + nOffset;
            break;
        default:
            break;
    }

    if (lPosition < 0) {
        lPosition = 0;
    } else if (lPosition >= this.nSizeInByte) {
        lPosition = this.nSizeInByte;
    }
    this.nPosition = lPosition;

    return this.nPosition;
}

static 
bool __c_string_io_eof( text_box_t *ptTextBox, 
                        uintptr_t pObj)
{
    text_box_c_str_reader_t *ptThis = (text_box_c_str_reader_t *)pObj;
    assert(NULL != ptThis);

    return this.nPosition >= this.nSizeInByte;

}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
