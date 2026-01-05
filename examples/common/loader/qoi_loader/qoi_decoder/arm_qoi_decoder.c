/*
 * Copyright (c) 2009-2025 Arm Limited. All rights reserved.
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
#define __QOI_DECODER_IMPLEMENT__
#include "arm_qoi_decoder.h"
#include "__arm_2d_impl.h"

#include <stdlib.h>
#include <stdio.h>


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
#   pragma clang diagnostic ignored "-Wtautological-pointer-compare"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wnonnull-compare"
#endif

/*============================ MACROS ========================================*/
#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

__STATIC_INLINE uint_fast8_t __arm_qoi_get_hash_index(uint32_t wPixelValue)
{
    __arm_qoi_pixel_t tPixel = {.wValue = wPixelValue};
    return (    (   tPixel.tColour.chRed * 3
                +   tPixel.tColour.chGreen * 5
                +   tPixel.tColour.chBlue * 7
                +   tPixel.tColour.chAlpha * 11)
           & ((1 << 6) - 1));                       /* mod 64 */
}

static
arm_2d_err_t __arm_qoi_reset_context(   arm_qoi_dec_t *ptThis, 
                                        arm_qoi_dec_ctx_t *ptContext, 
                                        bool bUpdateHeader)
{
    assert(NULL != ptThis);
    assert(NULL != ptContext);
    assert(NULL != this.tCFG.IO.fnRead);
    assert(NULL != this.tCFG.IO.fnSeek);

    /* initialize the working memory */
    memset(ptContext, 0, sizeof(arm_qoi_dec_ctx_t));

    /* get word-aligned working memory limit address */
    uintptr_t pnAddress = ((uintptr_t)this.tCFG.pchWorkingMemory + this.tCFG.hwSize)
                        & (~(uintptr_t)0x03);

    /* get the context address */
    pnAddress -= sizeof(arm_qoi_dec_ctx_t);

    /* sign */
    ptContext->ptQOIDec = ptThis;

#if ARM_QOI_IO_BUFF_SIZE > 0
    size_t tBufferSize = pnAddress - (uintptr_t)this.tCFG.pchWorkingMemory;
    ptContext->hwSize = MIN(__UINT16_MAX__, tBufferSize);
#endif

    struct __arm_qoi_header tQOIHeader;

    /* move to the head */
    if (!this.tCFG.IO.fnSeek(this.tCFG.pTarget, 0, SEEK_SET)) {
        return ARM_2D_ERR_IO_ERROR;
    }

    if (14 > this.tCFG.IO.fnRead(this.tCFG.pTarget, (uint8_t *)&tQOIHeader, 14 )) {
        return ARM_2D_ERR_IO_ERROR;
    }

    if (0 != memcmp("qoif", tQOIHeader.chMagic, 4)) {
        return ARM_2D_ERR_IO_ERROR;
    }

    ptContext->tPosition = 14;

    /* set previous pixel as {0, 0, 0, 0xFF} */
    ptContext->tPrevious.wValue = 0xFF000000;

    if (bUpdateHeader) {
        uint32_t wWidth = __rev(tQOIHeader.wWidth);
        uint32_t wHeight = __rev(tQOIHeader.wHeight);

        if (wWidth > __INT16_MAX__ || wHeight > __INT16_MAX__) {
            return ARM_2D_ERR_NOT_SUPPORT;
        }

        this.tSize.iWidth = (int16_t)wWidth;
        this.tSize.iHeight = (int16_t)wHeight;
        this.chChannels = tQOIHeader.chChannels;
    }

    if (3 == tQOIHeader.chChannels) {
        this.tCFG.bBlendWithBG = false;
    }

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1,2)
arm_2d_err_t arm_qoi_decoder_init(arm_qoi_dec_t *ptThis, arm_qoi_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);

    memset(ptThis, 0, sizeof(arm_qoi_dec_t));
    this.tCFG = *ptCFG;

    uintptr_t pnAddress = ((uintptr_t)this.tCFG.pchWorkingMemory + this.tCFG.hwSize)
                        & (~(uintptr_t)0x03);

    /* get the context address */
    pnAddress -= sizeof(arm_qoi_dec_ctx_t);

    if ((pnAddress < (uintptr_t)this.tCFG.pchWorkingMemory) || NULL == this.tCFG.pchWorkingMemory) {
        return ARM_2D_ERR_INVALID_PARAM;
    } else if (NULL == this.tCFG.IO.fnRead || NULL == this.tCFG.IO.fnSeek) {
        return ARM_2D_ERR_MISSING_PARAM;
    }

    if (this.tCFG.chOutputColourFormat > __ARM_QOI_DEC_FORMAT_VALID) {
        return ARM_2D_ERR_NOT_SUPPORT;
    }

    /* initialize the working memory */
    memset(this.tCFG.pchWorkingMemory, 0, this.tCFG.hwSize);

    /* get word-aligned working memory limit address */

    /* set working context */
    this.ptWorking = (arm_qoi_dec_ctx_t *)pnAddress;

    /* reset context */
    do {
        arm_2d_err_t tErr = 
            __arm_qoi_reset_context(ptThis, this.ptWorking, true);
        if (ARM_2D_ERR_NONE != tErr) {
            return tErr;
        }
    } while(0);
    
    this.bValid = true;
    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1,2)
arm_qoi_dec_ctx_t *arm_qoi_dec_save_context(arm_qoi_dec_t *ptThis, 
                                            arm_qoi_dec_ctx_t *ptContextOut)
{
    assert(NULL != ptThis);
    assert(NULL != ptContextOut);

    if (!this.bValid || NULL == this.ptWorking) {
        return NULL;
    }

    *ptContextOut = *this.ptWorking;
    return ptContextOut;
}


ARM_NONNULL(1,2)
arm_2d_err_t arm_qoi_dec_resume_context(arm_qoi_dec_t *ptThis, 
                                        arm_qoi_dec_ctx_t *ptContextOut)
{
    assert(NULL != ptThis);
    assert(NULL != ptContextOut);

    if (!this.bValid || NULL == this.ptWorking) {
        return ARM_2D_ERR_INVALID_STATUS;
    } else if (ptContextOut->ptQOIDec != ptThis) {
        /* invalid context */
        return ARM_2D_ERR_INVALID_PARAM;
    }

    *this.ptWorking = *ptContextOut;
    
    /* resume read position */
    if (!this.tCFG.IO.fnSeek(this.tCFG.pTarget, 
                             this.ptWorking->tPosition, 
                             SEEK_SET)) {
        return ARM_2D_ERR_IO_ERROR;
    }

#if ARM_QOI_IO_BUFF_SIZE > 0
    if (this.ptWorking->hwSize > 0) {
        /* refill the buffer */
        if (this.ptWorking->hwTail
         >  this.tCFG.IO.fnRead(this.tCFG.pTarget, 
                                this.tCFG.pchWorkingMemory,
                                this.ptWorking->hwTail)) {
            return ARM_2D_ERR_IO_ERROR;
        }
    }
#endif

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1,2) 
__STATIC_INLINE
size_t __arm_qoi_dec_io_read(arm_qoi_dec_t *ptThis, uint8_t *pchBuffer, size_t tSize)
{
    //assert(NULL != ptThis);
    //assert(NULL != pchBuffer);
    //assert(0 != tSize);
    //assert(NULL != this.tCFG.IO.fnRead);

#if 0 == ARM_QOI_IO_BUFF_SIZE 
    //if (0 == this.ptWorking->hwSize) {
        /* no buffer */
        tSize = this.tCFG.IO.fnRead(this.tCFG.pTarget, pchBuffer, tSize);
        this.ptWorking->tPosition += tSize;
        return tSize;
    //}
#else

    size_t hwLeftToRead = this.ptWorking->hwTail - this.ptWorking->hwHead;

    if (0 == hwLeftToRead) {

        /* calculate new postion */
        size_t tNewPosition = this.ptWorking->tPosition + this.ptWorking->hwTail;

        /* nothing to read */
        this.ptWorking->hwTail = 
            this.tCFG.IO.fnRead(this.tCFG.pTarget, 
                                this.tCFG.pchWorkingMemory, 
                                this.ptWorking->hwSize);
        this.ptWorking->hwHead = 0;

        if (0 == this.ptWorking->hwTail) {
            /* failed to fill the buffer, try next time */
            return 0;
        }

        this.ptWorking->tPosition = tNewPosition;
        hwLeftToRead = this.ptWorking->hwTail - this.ptWorking->hwHead;
    }

    tSize = MIN(hwLeftToRead, tSize);
    
    /* read memory */
    memcpy( pchBuffer, 
            &this.tCFG.pchWorkingMemory[this.ptWorking->hwHead],
            tSize);

    /* update buffer */
    this.ptWorking->hwHead += tSize;

    return tSize;
#endif
}


ARM_NONNULL(1,2) 
__STATIC_INLINE
size_t __arm_qoi_dec_io_try_to_read_word(arm_qoi_dec_t *ptThis, uint8_t *pchBuffer)
{
    //assert(NULL != ptThis);
    //assert(NULL != pchBuffer);
    //assert(0 != tSize);
    //assert(NULL != this.tCFG.IO.fnRead);
    size_t tSize = 4;
#if 0 == ARM_QOI_IO_BUFF_SIZE 
    //if (0 == this.ptWorking->hwSize) {

        /* no buffer */
        tSize = this.tCFG.IO.fnRead(this.tCFG.pTarget, pchBuffer, 4);
        this.ptWorking->tPosition += tSize;
        return tSize;
    //}
#else
    size_t hwLeftToRead = this.ptWorking->hwTail - this.ptWorking->hwHead;

    if (0 == hwLeftToRead) {

        /* calculate new postion */
        size_t tNewPosition = this.ptWorking->tPosition + this.ptWorking->hwTail;

        /* nothing to read */
        this.ptWorking->hwTail = 
            this.tCFG.IO.fnRead(this.tCFG.pTarget, 
                                this.tCFG.pchWorkingMemory, 
                                this.ptWorking->hwSize);
        this.ptWorking->hwHead = 0;

        if (0 == this.ptWorking->hwTail) {
            /* failed to fill the buffer, try next time */
            return 0;
        }

        this.ptWorking->tPosition = tNewPosition;
        hwLeftToRead = this.ptWorking->hwTail - this.ptWorking->hwHead;
    }

    tSize = MIN(hwLeftToRead, 4);
    
    if (4 == tSize) {
        /* provide sufficient information: 
         * 1. word-aligned target address
         * 2. 4 bytes to copy
         * 3. byte aligned source address
         *
         * The compiler might generate non-aligned word LDR STR pair in some
         * architucrue, i.e. Armv7-M etc.
         */
        memcpy( (uint32_t *)pchBuffer, 
                &this.tCFG.pchWorkingMemory[this.ptWorking->hwHead],
                4);
    } else {
        /* read memory */
        memcpy( pchBuffer, 
                &this.tCFG.pchWorkingMemory[this.ptWorking->hwHead],
                tSize);
    }

    /* update buffer */
    this.ptWorking->hwHead += tSize;

    return tSize;
#endif
}

ARM_NONNULL(1,2) 
__STATIC_INLINE
bool __arm_qoi_dec_io_read_byte(arm_qoi_dec_t *ptThis, uint8_t *pchBuffer)
{
    //assert(NULL != ptThis);
    //assert(NULL != pchBuffer);
    //assert(0 != tSize);
    //assert(NULL != this.tCFG.IO.fnRead);

#if 0 == ARM_QOI_IO_BUFF_SIZE 
    //if (0 == this.ptWorking->hwSize) {
        /* no buffer */
        if (this.tCFG.IO.fnRead(this.tCFG.pTarget, pchBuffer, 1)) {
            this.ptWorking->tPosition++;
            return true;
        } else {
            return false;
        }
    //}
#else
    size_t hwLeftToRead = this.ptWorking->hwTail - this.ptWorking->hwHead;

    if (0 == hwLeftToRead) {

        /* calculate new postion */
        size_t tNewPosition = this.ptWorking->tPosition + this.ptWorking->hwTail;

        /* nothing to read */
        this.ptWorking->hwTail = 
            this.tCFG.IO.fnRead(this.tCFG.pTarget, 
                                this.tCFG.pchWorkingMemory, 
                                this.ptWorking->hwSize);
        this.ptWorking->hwHead = 0;

        if (0 == this.ptWorking->hwTail) {
            /* failed to fill the buffer, try next time */
            return 0;
        }

        this.ptWorking->tPosition = tNewPosition;
    }

    *pchBuffer = this.tCFG.pchWorkingMemory[this.ptWorking->hwHead++];

    return true;
#endif
}


ARM_NONNULL(1,2) 
__STATIC_INLINE
bool __arm_qoi_read_data(arm_qoi_dec_t *ptThis, uint8_t *pchBuffer, size_t tSize)
{
    //assert(NULL != ptThis);
    //assert(NULL != pchBuffer);
    //assert(0 != tSize);

    do {
        size_t tActualRead = __arm_qoi_dec_io_read(ptThis, pchBuffer, tSize);
        if (tSize == tActualRead) {
            return true;
        } else if (0 == tActualRead) {
            return false;
        }

        tSize -= tActualRead;
        pchBuffer += tActualRead;
    } while(tSize);

    return true;
}

ARM_NONNULL(1,2) 
__STATIC_INLINE
bool __arm_qoi_read_word(arm_qoi_dec_t *ptThis, uint32_t *pwBuffer)
{
    //assert(NULL != ptThis);
    //assert(NULL != pchBuffer);
    //assert(0 != tSize);

    size_t tActualRead = __arm_qoi_dec_io_try_to_read_word(ptThis, (uint8_t *)pwBuffer);
    if (4 == tActualRead) {
        return true;
    } else if (0 == tActualRead) {
        return false;
    }

    return __arm_qoi_read_data( ptThis, 
                                ((uint8_t *)pwBuffer) + tActualRead,
                                4 - tActualRead);

}

__STATIC_INLINE
ARM_NONNULL(1) 
bool __arm_qoi_get_next_pixel(arm_qoi_dec_t *ptThis, __arm_qoi_pixel_t *ptPixel)
{
    //assert(NULL != ptThis);
    //assert(NULL != ptPixel);

    if (this.ptWorking->chRunCount) {
        this.ptWorking->chRunCount--;

        ptPixel->wValue = this.ptWorking->tPrevious.wValue;
        this.ptWorking->tPixelDecoded++;
        return true;
    }

    __arm_qoi_op_t tChunk;
    if (!__arm_qoi_dec_io_read_byte(ptThis, (uint8_t *)&tChunk)) {
        /* data is not available */
        return false;
    }
    __arm_qoi_pixel_t tPixel = { 
        .wValue = this.ptWorking->tPrevious.wValue, 
    };

    if (ARM_QOI_OP_ID_RGBA == tChunk.chID) {
    
        /* swap blue and red */
        if (!__arm_qoi_read_word(ptThis, &tPixel.wValue)) {
            /* data is not available */
            return false;
        }

        /* swap Red and Blue */
        uint32_t wValue = __rev16(tPixel.wValue);
        tPixel.wValue = __ror(wValue, 24);

    } else if (ARM_QOI_OP_ID_RGB == tChunk.chID) {

        if (!__arm_qoi_read_data(ptThis, (uint8_t *)&tPixel, 3)) {
            /* data is not available */
            return false;
        }
        /* swap Red and Blue */
        uint32_t wValue = __rev16(tPixel.wValue);
        tPixel.wValue = __ror(wValue, 24);

    } else switch (tChunk.u2OP) {
        case ARM_QOI_OP_INDEX: {
                __arm_qoi_op_index_t tOPIndex = {
                    .tHead.chID = tChunk.chID
                };
                tPixel.wValue = this.ptWorking->tHashTable[tOPIndex.u6Index].wValue;
            }
            break;
        case ARM_QOI_OP_DIFF: {
                __arm_qoi_op_diff_t tOPDiff = {
                    .tHead.chID = tChunk.chID
                };
                tPixel.tColour.chBlue += (int8_t)tOPDiff.u2DB - 2;
                tPixel.tColour.chGreen += (int8_t)tOPDiff.u2DG - 2;
                tPixel.tColour.chRed += (int8_t)tOPDiff.u2DR - 2;
            }
            break;
        case ARM_QOI_OP_LUMA: {
                __arm_qoi_op_luma_t tOPLuma = {
                    .tHead.chID = tChunk.chID
                };
                /* load params */
                if (!__arm_qoi_dec_io_read_byte(ptThis, ((uint8_t *)&tOPLuma) + 1)) {
                    /* data is not available */
                    return false;
                }

                int8_t chDG = (int8_t)tOPLuma.u6DG - 32;

                tPixel.tColour.chGreen += chDG;
                tPixel.tColour.chBlue += ((int8_t)tOPLuma.u4DBmDG - 8 + chDG);
                tPixel.tColour.chRed += ((int8_t)tOPLuma.u4DRmDG - 8 + chDG);
            }
            break;
        case ARM_QOI_OP_RUN: {
                __arm_qoi_op_run_t tOPRun = {
                    .tHead.chID = tChunk.chID
                };
                this.ptWorking->chRunCount = tOPRun.u6Run;
            }
            break;
    }

    this.ptWorking->tHashTable[__arm_qoi_get_hash_index(tPixel.wValue)].wValue = tPixel.wValue;
    this.ptWorking->tPrevious.wValue = tPixel.wValue;

    ptPixel->wValue = tPixel.wValue;
    this.ptWorking->tPixelDecoded++;

    return true;
}

__STATIC_INLINE
void __arm_qoi_decode_report_grid(  arm_qoi_dec_t *ptThis, 
                                    arm_2d_location_t *ptLocation,
                                    bool bShowReference)
{
    if (bShowReference && !(ptLocation->iX & 0x0F)) {
        ARM_2D_INVOKE_RT_VOID(
            this.tCFG.IO.fnReport, 
            ARM_2D_PARAM(   this.tCFG.pTarget, 
                            ptThis, 
                            *ptLocation,
                            this.ptWorking, 
                            ARM_QOI_CTX_REPORT_REF_GRID));
    }
}

ARM_NONNULL(1)
static
arm_2d_err_t __arm_qoi_decode_stride_rgb565(arm_qoi_dec_t *ptThis, 
                                            uint16_t *phwTarget,
                                            size_t tLength,
                                            arm_2d_location_t *ptLocation,
                                            bool bShowReference)
{
    assert(NULL != ptThis);
    assert(this.bValid);
    assert(NULL != ptLocation);
    
    if (0 == tLength || NULL == phwTarget) {
        return ARM_2D_ERR_NONE;
    }

    if (!bShowReference) {
        do {
            __arm_qoi_pixel_t tPixel;

            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            arm_2d_color_rgb565_t tPixelRGB565 = {
                .u5B = tPixel.tColour.chBlue >> 3,
                .u6G = tPixel.tColour.chGreen >> 2,
                .u5R = tPixel.tColour.chRed >> 3,
            };

            *phwTarget++ = tPixelRGB565.tValue;

        } while(--tLength);
    } else {
        do {
            __arm_qoi_pixel_t tPixel;

            __arm_qoi_decode_report_grid(ptThis, ptLocation, bShowReference);

            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            arm_2d_color_rgb565_t tPixelRGB565 = {
                .u5B = tPixel.tColour.chBlue >> 3,
                .u6G = tPixel.tColour.chGreen >> 2,
                .u5R = tPixel.tColour.chRed >> 3,
            };

            *phwTarget++ = tPixelRGB565.tValue;

            ptLocation->iX++;
        } while(--tLength);
    }

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
static
arm_2d_err_t __arm_qoi_decode_stride_gray8(arm_qoi_dec_t *ptThis, 
                                            uint8_t *pchTarget,
                                            size_t tLength,
                                            arm_2d_location_t *ptLocation,
                                            bool bShowReference)
{
    assert(NULL != ptThis);
    assert(this.bValid);
    assert(NULL != ptLocation);
    
    if (0 == tLength || NULL == pchTarget) {
        return ARM_2D_ERR_NONE;
    }

    uint8_t chInvertMask = this.tCFG.bInvertColour ? 0xFF : 00;

    if (!bShowReference) {
        do {
            __arm_qoi_pixel_t tPixel;

            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            __arm_2d_color_fast_rgb_t tColour;
            __arm_2d_ccca8888_unpack(tPixel.wValue, &tColour);
            *pchTarget++ = __arm_2d_gray8_pack(&tColour) ^ chInvertMask;

        } while(--tLength);
    } else {
        do {
            __arm_qoi_pixel_t tPixel;

            __arm_qoi_decode_report_grid(ptThis, ptLocation, bShowReference);

            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            __arm_2d_color_fast_rgb_t tColour;
            __arm_2d_ccca8888_unpack(tPixel.wValue, &tColour);
            *pchTarget++ = __arm_2d_gray8_pack(&tColour) ^ chInvertMask;
            ptLocation->iX++;
        } while(--tLength);
    }

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
static
arm_2d_err_t __arm_qoi_decode_stride_gray8_with_background(arm_qoi_dec_t *ptThis, 
                                            uint8_t *pchTarget,
                                            size_t tLength,
                                            arm_2d_location_t *ptLocation,
                                            bool bShowReference)
{
    assert(NULL != ptThis);
    assert(this.bValid);
    assert(NULL != ptLocation);
    
    if (0 == tLength || NULL == pchTarget) {
        return ARM_2D_ERR_NONE;
    }

    uint8_t chInvertMask = this.tCFG.bInvertColour ? 0xFF : 00;

    if (!bShowReference) {
        do {
            __arm_qoi_pixel_t tPixel;

            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            __arm_2d_color_fast_rgb_t tColour;
            __arm_2d_ccca8888_unpack(tPixel.wValue, &tColour);

            uint16_t hwPixel = __arm_2d_gray8_pack(&tColour) ^ chInvertMask;

            uint16_t hwOpa= tPixel.tColour.chAlpha;
            hwOpa += (hwOpa == 255);
            uint16_t hwTrans = 256 - hwOpa;

            *pchTarget = (hwPixel * hwOpa + (uint16_t)*pchTarget * hwTrans) >> 8;
            pchTarget++;

        } while(--tLength);
    } else {
        do {
            __arm_qoi_pixel_t tPixel;

            __arm_qoi_decode_report_grid(ptThis, ptLocation, bShowReference);

            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            __arm_2d_color_fast_rgb_t tColour;
            __arm_2d_ccca8888_unpack(tPixel.wValue, &tColour);

            uint16_t hwPixel = __arm_2d_gray8_pack(&tColour) ^ chInvertMask;

            uint16_t hwOpa= tPixel.tColour.chAlpha;
            hwOpa += (hwOpa == 255);
            uint16_t hwTrans = 256 - hwOpa;

            *pchTarget = (hwPixel * hwOpa + (uint16_t)*pchTarget * hwTrans) >> 8;
            pchTarget++;

            ptLocation->iX++;

        } while(--tLength);
    }

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
static
arm_2d_err_t __arm_qoi_decode_stride_channel(   arm_qoi_dec_t *ptThis, 
                                                uint8_t *pchTarget,
                                                size_t tLength,
                                                uint_fast8_t chIndex,
                                                arm_2d_location_t *ptLocation,
                                                bool bShowReference)
{
    assert(NULL != ptThis);
    assert(this.bValid);
    assert(NULL != ptLocation);
    
    if (0 == tLength || NULL == pchTarget) {
        return ARM_2D_ERR_NONE;
    }

    uint8_t chInvertMask = this.tCFG.bInvertColour ? 0xFF : 00;

    if (!bShowReference) {
        do {
            __arm_qoi_pixel_t tPixel;

            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            *pchTarget++ = tPixel.tColour.chChannel[chIndex] ^ chInvertMask;
        } while(--tLength);
    } else {
        do {
            __arm_qoi_pixel_t tPixel;

            __arm_qoi_decode_report_grid(ptThis, ptLocation, bShowReference);

            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            *pchTarget++ = tPixel.tColour.chChannel[chIndex] ^ chInvertMask;
            ptLocation->iX++;
        } while(--tLength);
    }

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
static
arm_2d_err_t __arm_qoi_decode_stride_rgb565_with_background(
                                            arm_qoi_dec_t *ptThis, 
                                            uint16_t *phwTarget,
                                            size_t tLength,
                                            arm_2d_location_t *ptLocation,
                                            bool bShowReference)
{
    assert(NULL != ptThis);
    assert(this.bValid);
    assert(NULL != ptLocation);
    
    if (0 == tLength || NULL == phwTarget) {
        return ARM_2D_ERR_NONE;
    }

    if (!bShowReference) {
        do {
            __arm_qoi_pixel_t tPixel;
            
            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            uint16_t hwOpa= tPixel.tColour.chAlpha;
            hwOpa += (hwOpa == 255);
            uint16_t hwTrans = 256 - hwOpa;

            __arm_2d_color_fast_rgb_t tColour;
            __arm_2d_rgb565_unpack(*phwTarget, &tColour);

            tColour.BGRA[0] = (tColour.BGRA[0] * hwTrans + (uint16_t)tPixel.tColour.chChannel[0] * hwOpa) >> 8;
            tColour.BGRA[1] = (tColour.BGRA[1] * hwTrans + (uint16_t)tPixel.tColour.chChannel[1] * hwOpa) >> 8;
            tColour.BGRA[2] = (tColour.BGRA[2] * hwTrans + (uint16_t)tPixel.tColour.chChannel[2] * hwOpa) >> 8;

            *phwTarget++ = __arm_2d_rgb565_pack(&tColour);
        } while(--tLength);
    } else {
        do {
            __arm_qoi_pixel_t tPixel;

            __arm_qoi_decode_report_grid(ptThis, ptLocation, bShowReference);
            
            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            uint16_t hwOpa= tPixel.tColour.chAlpha;
            hwOpa += (hwOpa == 255);
            uint16_t hwTrans = 256 - hwOpa;

            __arm_2d_color_fast_rgb_t tColour;
            __arm_2d_rgb565_unpack(*phwTarget, &tColour);

            tColour.BGRA[0] = (tColour.BGRA[0] * hwTrans + (uint16_t)tPixel.tColour.chChannel[0] * hwOpa) >> 8;
            tColour.BGRA[1] = (tColour.BGRA[1] * hwTrans + (uint16_t)tPixel.tColour.chChannel[1] * hwOpa) >> 8;
            tColour.BGRA[2] = (tColour.BGRA[2] * hwTrans + (uint16_t)tPixel.tColour.chChannel[2] * hwOpa) >> 8;

            *phwTarget++ = __arm_2d_rgb565_pack(&tColour);
            ptLocation->iX++;

        } while(--tLength);
    }

    return ARM_2D_ERR_NONE;
}



ARM_NONNULL(1)
static
arm_2d_err_t __arm_qoi_decode_stride_ccca8888(  arm_qoi_dec_t *ptThis, 
                                                uint32_t *pwTarget,
                                                size_t tLength,
                                                arm_2d_location_t *ptLocation,
                                                bool bShowReference)
{
    assert(NULL != ptThis);
    assert(this.bValid);
    assert(NULL != ptLocation);
    
    if (0 == tLength || NULL == pwTarget) {
        return ARM_2D_ERR_NONE;
    }

    if (!bShowReference) {
        do {
            __arm_qoi_pixel_t tPixel;
            
            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            *pwTarget++ = tPixel.wValue;
        } while(--tLength);
    } else {
        do {
            __arm_qoi_pixel_t tPixel;

            __arm_qoi_decode_report_grid(ptThis, ptLocation, bShowReference);
            
            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            *pwTarget++ = tPixel.wValue;
            ptLocation->iX++;
        } while(--tLength);
    }

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
static
arm_2d_err_t __arm_qoi_decode_stride_cccn888_with_background(
                                            arm_qoi_dec_t *ptThis, 
                                            uint32_t *pwTarget,
                                            size_t tLength,
                                            arm_2d_location_t *ptLocation,
                                            bool bShowReference)
{
    assert(NULL != ptThis);
    assert(this.bValid);
    assert(NULL != ptLocation);
    
    if (0 == tLength || NULL == pwTarget) {
        return ARM_2D_ERR_NONE;
    }

    if (!bShowReference) {
        do {
            __arm_qoi_pixel_t tPixel;

            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            uint16_t hwOpa= tPixel.tColour.chAlpha;
            hwOpa += (hwOpa == 255);
            uint16_t hwTrans = 256 - hwOpa;

            __arm_2d_color_fast_rgb_t tColour;
            __arm_2d_ccca8888_unpack(*pwTarget, &tColour);

            tColour.BGRA[0] = (tColour.BGRA[0] * hwTrans + (uint16_t)tPixel.tColour.chChannel[0] * hwOpa) >> 8;
            tColour.BGRA[1] = (tColour.BGRA[1] * hwTrans + (uint16_t)tPixel.tColour.chChannel[1] * hwOpa) >> 8;
            tColour.BGRA[2] = (tColour.BGRA[2] * hwTrans + (uint16_t)tPixel.tColour.chChannel[2] * hwOpa) >> 8;

            *pwTarget++ = __arm_2d_ccca8888_pack(&tColour);

        } while(--tLength);
    } else {
        do {
            __arm_qoi_pixel_t tPixel;

            __arm_qoi_decode_report_grid(ptThis, ptLocation, bShowReference);

            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }

            uint16_t hwOpa= tPixel.tColour.chAlpha;
            hwOpa += (hwOpa == 255);
            uint16_t hwTrans = 256 - hwOpa;

            __arm_2d_color_fast_rgb_t tColour;
            __arm_2d_ccca8888_unpack(*pwTarget, &tColour);

            tColour.BGRA[0] = (tColour.BGRA[0] * hwTrans + (uint16_t)tPixel.tColour.chChannel[0] * hwOpa) >> 8;
            tColour.BGRA[1] = (tColour.BGRA[1] * hwTrans + (uint16_t)tPixel.tColour.chChannel[1] * hwOpa) >> 8;
            tColour.BGRA[2] = (tColour.BGRA[2] * hwTrans + (uint16_t)tPixel.tColour.chChannel[2] * hwOpa) >> 8;

            *pwTarget++ = __arm_2d_ccca8888_pack(&tColour);
            ptLocation->iX++;

        } while(--tLength);
    }

    return ARM_2D_ERR_NONE;
}

__STATIC_FORCEINLINE
arm_2d_location_t *__arm_qoi_decoder_get_context_location(  arm_qoi_dec_t *ptThis,
                                                            arm_qoi_dec_ctx_t *ptContext,
                                                            arm_2d_location_t *ptLocationOut)
{
    int16_t iStride = this.tSize.iWidth;
    ptLocationOut->iY = ptContext->tPixelDecoded / iStride;
    ptLocationOut->iX = (size_t)ptContext->tPixelDecoded - (size_t)ptLocationOut->iY * (size_t)iStride;
    return ptLocationOut;
}

__STATIC_INLINE
arm_2d_location_t *__arm_qoi_decoder_get_current_location(  arm_qoi_dec_t *ptThis, 
                                                            arm_2d_location_t *ptLocationOut)
{
    return __arm_qoi_decoder_get_context_location(ptThis, this.ptWorking, ptLocationOut);
}

ARM_NONNULL(1,3)
arm_2d_location_t *arm_qoi_decoder_get_context_location(arm_qoi_dec_t *ptThis, 
                                                        arm_qoi_dec_ctx_t *ptContext,
                                                        arm_2d_location_t *ptLocationOut)
{
    assert(NULL != ptThis);
    assert(NULL != ptLocationOut);

    if (!this.bValid) {
        return NULL;
    }

    if (NULL == ptContext) {
        ptContext = this.ptWorking;
    }

    return __arm_qoi_decoder_get_context_location(ptThis, ptContext, ptLocationOut);
}

ARM_NONNULL(1)
size_t arm_qoi_decoder_get_context_pixel_number(arm_qoi_dec_ctx_t *ptContext)
{
    assert(NULL != ptContext);

    return ptContext->tPixelDecoded;
}

ARM_NONNULL(1)
bool arm_qoi_decoder_is_pre_blend_with_background(arm_qoi_dec_t *ptThis)
{
    assert(NULL != ptThis);

    return this.tCFG.bBlendWithBG;
}

ARM_NONNULL(1)
arm_qoi_dec_ctx_t * arm_qoi_decoder_get_current_context(arm_qoi_dec_t *ptThis)
{
    assert(NULL != ptThis);
    return this.ptWorking;
}

ARM_NONNULL(1,2)
arm_2d_err_t arm_qoi_decode(arm_qoi_dec_t *ptThis,
                            void *pTarget,
                            arm_2d_region_t *ptTargetRegion,
                            size_t tTargetStrideInByte)
{
    assert(NULL != ptThis);
    assert(NULL != pTarget);

    uintptr_t nTargetAddress = (uintptr_t)pTarget;

    if (!this.bValid) {
        return ARM_2D_ERR_NOT_AVAILABLE;
    }

    arm_2d_region_t tTargetRegion = {
        .tSize = this.tSize,
    };

    if (NULL != ptTargetRegion) {
        if (!arm_2d_region_intersect(&tTargetRegion, ptTargetRegion, &tTargetRegion)) {
            return ARM_2D_ERR_OUT_OF_REGION;
        }
    }

    int16_t iStride = this.tSize.iWidth;
    
    int16_t x = 0, y = 0, iTargetStride = tTargetRegion.tSize.iWidth;

    __arm_qoi_pixel_t tPixel;
    arm_2d_location_t tCurrentLocation;
    __arm_qoi_decoder_get_current_location(ptThis, &tCurrentLocation);

    y = tCurrentLocation.iY;
    x = tCurrentLocation.iX;

    /* decoding region of interest */
    int16_t iHeight = tTargetRegion.tSize.iHeight + tTargetRegion.tLocation.iY;

    bool bFirstLine = true;
    bool bNeedToReport = (NULL != this.tCFG.IO.fnReport);
    bool bReferenceY = !(y & 0x0F) && bNeedToReport;
    
    if (y < tTargetRegion.tLocation.iY) {
        goto entry_skip_headroom;
    } else if (x < tTargetRegion.tLocation.iX && y == tTargetRegion.tLocation.iY) {
        goto entry_skip_left;
    } else if (x == tTargetRegion.tLocation.iX && y == tTargetRegion.tLocation.iY) {
        goto entry_decode;
    } else {
        /* we have to reset working context */
        arm_2d_err_t tErr = 
            __arm_qoi_reset_context(ptThis, this.ptWorking, false);
        if (ARM_2D_ERR_NONE != tErr) {
            return tErr;
        }
        x = 0;
        y = 0;
    }

    /* start decoding from the very begining */
    ARM_2D_INVOKE_RT_VOID(
        this.tCFG.IO.fnReport, 
        ARM_2D_PARAM(   this.tCFG.pTarget, 
                        ptThis, 
                        tCurrentLocation,
                        this.ptWorking, 
                        ARM_QOI_CTX_REPORT_START));

    /* skip headroom */
    for (; y < tTargetRegion.tLocation.iY; y++) {
        for (x = 0; x < iStride; x++) {
entry_skip_headroom: 
            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }
        }
    }

    do {
        bReferenceY = !(y & 0x0F) && bNeedToReport;

        /* skip left */
        for (x = 0; x < tTargetRegion.tLocation.iX; x++) {
entry_skip_left:
            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }
        }
    
entry_decode:
        if (bFirstLine) {
            /* report reach the top left corner */
            ARM_2D_INVOKE_RT_VOID(this.tCFG.IO.fnReport, 
                ARM_2D_PARAM(   this.tCFG.pTarget, 
                                ptThis, 
                                (arm_2d_location_t){x, y},
                                this.ptWorking, 
                                ARM_QOI_CTX_REPORT_TOP_LEFT));
        }
        
        iTargetStride = tTargetRegion.tSize.iWidth;

        tCurrentLocation.iX = x;
        tCurrentLocation.iY = y;

        arm_2d_err_t tResult;
        switch (this.tCFG.chOutputColourFormat) {
            case ARM_QOI_DEC_FORMAT_CCCA8888:
                tResult = __arm_qoi_decode_stride_ccca8888( 
                                                    ptThis, 
                                                    (uint32_t *)nTargetAddress,
                                                    iTargetStride,
                                                    &tCurrentLocation,
                                                    bReferenceY);
                break;
            case ARM_QOI_DEC_FORMAT_CCCN888:
                if (this.tCFG.bBlendWithBG) {
                    tResult = __arm_qoi_decode_stride_cccn888_with_background(
                                ptThis, 
                                (uint32_t *)nTargetAddress, 
                                iTargetStride,
                                &tCurrentLocation,
                                bReferenceY);
                } else {
                    tResult = __arm_qoi_decode_stride_ccca8888(
                                ptThis, 
                                (uint32_t *)nTargetAddress, 
                                iTargetStride,
                                &tCurrentLocation,
                                bReferenceY);
                }
                break;
            case ARM_QOI_DEC_FORMAT_RGB565:
                if (this.tCFG.bBlendWithBG) {
                    tResult = __arm_qoi_decode_stride_rgb565_with_background(
                                ptThis, 
                                (uint16_t *)nTargetAddress, 
                                iTargetStride,
                                &tCurrentLocation,
                                bReferenceY);
                } else {
                    tResult = __arm_qoi_decode_stride_rgb565(
                                ptThis, 
                                (uint16_t *)nTargetAddress, 
                                iTargetStride,
                                &tCurrentLocation,
                                bReferenceY);
                }
                break;
            case ARM_QOI_DEC_FORMAT_GRAY8:
                if (this.tCFG.bBlendWithBG) {
                    tResult = __arm_qoi_decode_stride_gray8_with_background(
                                ptThis, 
                                (uint8_t *)nTargetAddress, 
                                iTargetStride,
                                &tCurrentLocation,
                                bReferenceY);
                } else {
                    tResult = __arm_qoi_decode_stride_gray8(
                                ptThis, 
                                (uint8_t *)nTargetAddress, 
                                iTargetStride,
                                &tCurrentLocation,
                                bReferenceY);
                }
                break;
            default: {
                    uint_fast8_t chIndex = this.tCFG.chOutputColourFormat - ARM_QOI_DEC_FORMAT_CHN_B;
                    tResult = __arm_qoi_decode_stride_channel(
                            ptThis,
                            (uint8_t *)nTargetAddress,
                            iTargetStride,
                            chIndex,
                            &tCurrentLocation,
                            bReferenceY);
                }
                break;
        }

        nTargetAddress += tTargetStrideInByte;

        if (ARM_2D_ERR_NONE != tResult) {
            return tResult;
        }
        x += iTargetStride;

        if (bFirstLine) {
            bFirstLine = false;
            /* report reach the top left corner */
            ARM_2D_INVOKE_RT_VOID(this.tCFG.IO.fnReport, 
                ARM_2D_PARAM(   this.tCFG.pTarget, 
                                ptThis, 
                                (arm_2d_location_t){x, y},
                                this.ptWorking, 
                                ARM_QOI_CTX_REPORT_TOP_RIGHT));
        }

        y++;
        if (y >= iHeight) {
            break;
        }

        for (; x < iStride; x++) {
            if (!__arm_qoi_get_next_pixel(ptThis, &tPixel)) {
                return ARM_2D_ERR_NOT_AVAILABLE;
            }
        }

    } while(true);

    __arm_qoi_decoder_get_current_location(ptThis, &tCurrentLocation);

    if (    this.ptWorking->tPixelDecoded 
       >=   (size_t)this.tSize.iHeight * (size_t)this.tSize.iWidth) {
        ARM_2D_INVOKE_RT_VOID(
            this.tCFG.IO.fnReport, 
            ARM_2D_PARAM(   this.tCFG.pTarget, 
                            ptThis, 
                            tCurrentLocation,
                            this.ptWorking, 
                            ARM_QOI_CTX_REPORT_END));
    }

    return ARM_2D_ERR_NONE;
}
