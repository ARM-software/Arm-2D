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

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static 
int32_t __c_string_io_read_utf8_char_handler_t(text_box_t *ptThis, 
                                            uintptr_t pObj, 
                                            uint8_t *pchBuffer,
                                            uint_fast16_t hwSize);

static 
int32_t __c_string_io_seek_handler_t(   text_box_t *ptThis, 
                                        uintptr_t pObj,
                                        int32_t nOffset,
                                        text_box_seek_whence_t enWhence);

/*============================ GLOBAL VARIABLES ==============================*/
const text_box_io_handler_t TEXT_BOX_IO_C_STRING_READER = {
    .fnGetChar = &__c_string_io_read_utf8_char_handler_t,
    .fnSeek = &__c_string_io_seek_handler_t,
};

/*============================ IMPLEMENTATION ================================*/

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

    this.Start.nLine = 0;

    if (    (NULL != ptCFG->ptScratchMemory)
       &&   (ptCFG->hwScratchMemoryCount > 0)) {

        uint_fast16_t hwCount = ptCFG->hwScratchMemoryCount;
        __text_box_scratch_mem_t *ptItem = ptCFG->ptScratchMemory;

        /* add scrach memory to pool */
        do {
            __scratch_memory_free(ptThis, ptItem++);
        } while(--hwCount);
    }
}

ARM_NONNULL(1)
void text_box_depose( text_box_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void text_box_on_load( text_box_t *ptThis)
{
    assert(NULL != ptThis);
    
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

        if (bIsNewFrame) {
            if (this.iLineWidth != __text_box_canvas.tSize.iWidth) {
                this.iLineWidth = __text_box_canvas.tSize.iWidth;

                text_box_update(ptThis);
            }
        }

        arm_2d_draw_box(&__text_box, NULL, 1, GLCD_COLOR_BLUE, 128);
    }

    ARM_2D_OP_WAIT_ASYNC();
}

typedef enum {
    TEXT_BOX_CHAR_NORMAL,
    TEXT_BOX_CHAR_SEPERATOR,
    TEXT_BOX_CHAR_END_OF_LINE,
    TEXT_BOX_CHAR_CUT_OFF,
} __text_box_char_type_t;

/*!
 * \brief detect bricks and update line info
 * \return bool whether detect a brick
 */
static
ARM_NONNULL(1)
bool __text_box_detect_brick(   uint8_t *pchPT, 
                                __text_box_char_type_t tType,
                                __text_box_line_info_t *ptLineInfo,
                                int16_t iLineWidth)
{
    assert(NULL != pchPT);

ARM_PT_BEGIN(*pchPT)

    /* wait for a normal char as the begin of tha brick */
    ARM_PT_ENTRY()
        if (tType != TEXT_BOX_CHAR_NORMAL) {
            ARM_PT_GOTO_PREV_ENTRY(false)
        }

    /* accept for normal chars */
    ARM_PT_ENTRY()
        if (tType == TEXT_BOX_CHAR_NORMAL) {
            ARM_PT_GOTO_PREV_ENTRY(false)
        }

        if (tType == TEXT_BOX_CHAR_CUT_OFF) {
            /* end a line and cut a brick off */
            ARM_PT_RETURN(false);
        }

        /* normal end of a brick */
        if (NULL != ptLineInfo) {
            ptLineInfo->hwBrickCount++;
            ptLineInfo->iLineWidth = iLineWidth;
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
    int16_t iFontCharWidth = this.tCFG.ptFont->tCharSize.iWidth;
    bool bGetAValidLine = false;
    arm_2d_char_descriptor_t tDescriptor;
    int16_t iWhiteSpaceWidth = 0;
    arm_2d_char_descriptor_t *ptDescriptor =
        arm_2d_helper_get_char_descriptor(this.tCFG.ptFont, 
            &tDescriptor, 
            tUTF8Char.chByte);
    if (NULL == ptDescriptor) {
        iWhiteSpaceWidth = iFontCharWidth;
    } else {
        iWhiteSpaceWidth = ptDescriptor->iAdvance;
    }

    do {
        bool bEndOfStream = false;
        tUTF8Char.wValue = 0;

        int32_t nActualReadSize = 
            ARM_2D_INVOKE(this.tCFG.tStreamIO.ptIO->fnGetChar,
                ARM_2D_PARAM(   ptThis, 
                                this.tCFG.tStreamIO.pTarget,
                                tUTF8Char.chByte,
                                4));

        if (nActualReadSize <= 0) {
            /* we reach end of the stream */
            break;
        } else if (nActualReadSize < 4) {
            bEndOfStream = true;
        }

        bGetAValidLine = true;

        nActualReadSize -= arm_2d_helper_get_utf8_byte_length(tUTF8Char.chByte);
        if (nActualReadSize > 0) {
            /* move back */
            ARM_2D_INVOKE(this.tCFG.tStreamIO.ptIO->fnSeek,
                ARM_2D_PARAM(   ptThis, 
                                this.tCFG.tStreamIO.pTarget,
                                -nActualReadSize,
                                TEXT_BOX_SEEK_CUR));
        }

        bool bNormalChar = true;
        switch (tUTF8Char.chByte[0]) {
            case '\n':
                bNormalChar = false;
                bEndOfStream = true;
                __text_box_detect_brick(&chBrickDetectorPT,
                                        TEXT_BOX_CHAR_END_OF_LINE,
                                        ptLineInfo,
                                        iLineWidth);

                break;
            case '\t':
                bNormalChar = false;
                iLineWidth += iWhiteSpaceWidth * 4;
                __text_box_detect_brick(&chBrickDetectorPT,
                                        TEXT_BOX_CHAR_SEPERATOR,
                                        ptLineInfo,
                                        iLineWidth);
                break;
            case ' ':
                /* detect a blank space */
                __text_box_detect_brick(&chBrickDetectorPT,
                                        TEXT_BOX_CHAR_SEPERATOR,
                                        ptLineInfo,
                                        iLineWidth);
                break;
            default:
                if (tUTF8Char.chByte[0] < 0x20) {
                    /* ignore invisible chars */
                    bNormalChar = false;
                    __text_box_detect_brick(&chBrickDetectorPT,
                                        TEXT_BOX_CHAR_SEPERATOR,
                                        ptLineInfo,
                                        iLineWidth);
                } else {
                    __text_box_detect_brick(&chBrickDetectorPT,
                                        TEXT_BOX_CHAR_NORMAL,
                                        ptLineInfo,
                                        iLineWidth);
                }
                break;
        }

        if (bNormalChar) {
            ptDescriptor = arm_2d_helper_get_char_descriptor(
                                                        this.tCFG.ptFont,
                                                        &tDescriptor, 
                                                        tUTF8Char.chByte);

            if (NULL == ptDescriptor) {
                iLineWidth += iFontCharWidth;
            } else {
                iLineWidth += ptDescriptor->iAdvance;
            }
        }

        if (iLineWidth + iFontCharWidth > this.iLineWidth) {
            /* insufficient space for next char */
            bEndOfStream = true;

            __text_box_detect_brick(&chBrickDetectorPT,
                                    TEXT_BOX_CHAR_CUT_OFF,
                                    ptLineInfo,
                                    iLineWidth);
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

            int16_t iResidualWidth = this.iLineWidth - ptLineInfo->iLineWidth;
            assert(iResidualWidth > 0);

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

ARM_NONNULL(1)
void text_box_update(text_box_t *ptThis)
{
    assert(NULL != ptThis);
    assert(NULL != this.tCFG.tStreamIO.ptIO);
    assert(NULL != this.tCFG.tStreamIO.ptIO->fnSeek);

    /* move to the begin */
    ARM_2D_INVOKE(this.tCFG.tStreamIO.ptIO->fnSeek,
        ARM_2D_PARAM(   ptThis, 
                        this.tCFG.tStreamIO.pTarget,
                        0,
                        TEXT_BOX_SEEK_SET));

    int32_t nLineNumber = 0;

    /* find the reading position of the start line */
    do {
        /* get the current position */
        if (this.Start.nLine == nLineNumber) {
            this.Start.nPosition = ARM_2D_INVOKE(this.tCFG.tStreamIO.ptIO->fnSeek,
                ARM_2D_PARAM(   ptThis, 
                                this.tCFG.tStreamIO.pTarget,
                                0,
                                TEXT_BOX_SEEK_CUR));
            break;
        }

        if (!__text_box_get_and_analyze_one_line(ptThis, NULL)) {
            /* failed to read a line */
            break;
        }

        nLineNumber++;
    } while(1);

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
int32_t __c_string_io_read_utf8_char_handler_t( text_box_t *ptTextBox, 
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
int32_t __c_string_io_seek_handler_t(   text_box_t *ptTextBox, 
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

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
