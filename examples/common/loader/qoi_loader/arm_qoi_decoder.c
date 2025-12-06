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

#include <stdlib.h>


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

ARM_NONNULL(1,2)
arm_2d_err_t arm_qoi_decoder_init(arm_qoi_dec_t *ptThis, arm_qoi_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);

    memset(ptThis, 0, sizeof(arm_qoi_dec_t));
    this.tCFG = *ptCFG;

    if (this.tCFG.hwSize < (sizeof(arm_qoi_dec_ctx_t) + 4) || NULL == this.tCFG.pchWorkingMemory) {
        return ARM_2D_ERR_INVALID_PARAM;
    } else if (NULL == this.tCFG.IO.fnRead || NULL == this.tCFG.IO.fnSeek) {
        return ARM_2D_ERR_MISSING_PARAM;
    }

    /* initialize the working memory */
    memset(this.tCFG.pchWorkingMemory, 0, this.tCFG.hwSize);

    /* get word-aligned working memory limit address */
    uintptr_t pnAddress = ((uintptr_t)this.tCFG.pchWorkingMemory + this.tCFG.hwSize)
                        & (~(uintptr_t)0x03);

    /* get the context address */
    pnAddress -= sizeof(arm_qoi_dec_ctx_t);
    /* set working context */
    this.ptWorking = (arm_qoi_dec_ctx_t *)pnAddress;

    /* sign */
    this.ptWorking->ptQOIDec = ptThis;

    size_t tBufferSize = (uintptr_t)this.tCFG.pchWorkingMemory - pnAddress;
    this.ptWorking->hwSize = MIN(__UINT16_MAX__, tBufferSize);

    /* read header */
    do {
        struct __arm_qoi_header tQOIHeader;

        /* move to the head */
        if (!this.tCFG.IO.fnSeek(this.pTarget, 0, SEEK_SET)) {
            return ARM_2D_ERR_IO_ERROR;
        }

        if (sizeof(tQOIHeader) > this.tCFG.IO.fnRead(this.pTarget, (uint8_t *)&tQOIHeader, sizeof(tQOIHeader) )) {
            return ARM_2D_ERR_IO_ERROR;
        }

        if (0 != memcmp("qoif", tQOIHeader.chMagic, 4)) {
            return ARM_2D_ERR_IO_ERROR;
        }

        uint32_t wWidth = __rev(tQOIHeader.wWidth);
        uint32_t wHeight = __rev(tQOIHeader.wHeight);

        if (wWidth > __INT16_MAX__ || wHeight > __INT16_MAX__) {
            return ARM_2D_ERR_NOT_SUPPORT;
        }

        this.tSize.iWidth = (int16_t)wWidth;
        this.tSize.iHeight = (int16_t)wHeight;
    } while(0);

    /* set previous pixel as {0, 0, 0, 0xFF} */
    this.ptWorking->tPrevious.wValue = 0xFF000000;
    
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
    if (!this.tCFG.IO.fnSeek(this.pTarget, 
                             this.ptWorking->tPosition, 
                             SEEK_SET)) {
        return ARM_2D_ERR_IO_ERROR;
    }

    if (this.ptWorking->hwSize > 0) {
        /* refill the buffer */
        if (this.ptWorking->hwTail
         >  this.tCFG.IO.fnRead(this.pTarget, 
                                this.tCFG.pchWorkingMemory,
                                this.ptWorking->hwTail)) {
            return ARM_2D_ERR_IO_ERROR;
        }
    }

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

    if (0 == this.ptWorking->hwSize) {
        /* no buffer */
        return this.tCFG.IO.fnRead(this.pTarget, pchBuffer, tSize); 
    }

    size_t hwLeftToRead = this.ptWorking->hwTail - this.ptWorking->hwHead;

    if (0 == hwLeftToRead) {

        /* calculate new postion */
        size_t tNewPosition = this.ptWorking->tPosition + this.ptWorking->hwTail;

        /* nothing to read */
        this.ptWorking->hwTail = 
            this.tCFG.IO.fnRead(this.pTarget, 
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
        if (0 == tActualRead) {
            this.chErrorCode = ARM_2D_ERR_IO_ERROR;
            return false;
        }

        tSize -= tActualRead;
        pchBuffer += tActualRead;
    } while(tSize);

    return true;
}

__STATIC_INLINE
ARM_NONNULL(1) 
bool __arm_qoi_get_next_pixel(arm_qoi_dec_t *ptThis, __arm_qoi_pixel_t *ptPixel)
{
    //assert(NULL != ptThis);
    //assert(NULL != ptPixel);

    if (this.ptWorking->u6RunCount) {
        this.ptWorking->u6RunCount--;

        ptPixel->wValue = this.ptWorking->tPrevious.wValue;
        this.ptWorking->tPixelDecoded++;
        return true;
    }

    __arm_qoi_op_t tChunk;
    if (!__arm_qoi_read_data(ptThis, (uint8_t *)&tChunk, 1)) {
        /* data is not available */
        return false;
    }
    __arm_qoi_pixel_t tPixel = { 
        .wValue = this.ptWorking->tPrevious.wValue, 
    };

    if (ARM_QOI_OP_ID_RGBA == tChunk.chID) {
       
        if (!__arm_qoi_read_data(ptThis, (uint8_t *)&tPixel, 4)) {
            /* data is not available */
            return false;
        }

    } else if (ARM_QOI_OP_ID_RGB == tChunk.chID) {
        if (!__arm_qoi_read_data(ptThis, (uint8_t *)&tPixel, 3)) {
            /* data is not available */
            return false;
        }

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
                tPixel.tColour.chBlue += (uint8_t)tOPDiff.u2DB - 2;
                tPixel.tColour.chGreen += (uint8_t)tOPDiff.u2DG - 2;
                tPixel.tColour.chRed += (uint8_t)tOPDiff.u2DR - 2;
            }
            break;
        case ARM_QOI_OP_LUMA: {
                __arm_qoi_op_luma_t tOPLuma = {
                    .tHead.chID = tChunk.chID
                };
                /* load params */
                if (!__arm_qoi_read_data(ptThis, ((uint8_t *)&tOPLuma) + 1, 1)) {
                    /* data is not available */
                    return false;
                }

                int8_t chDG = tOPLuma.s6DG;

                tPixel.tColour.chGreen += chDG;
                tPixel.tColour.chBlue += ((int8_t)tOPLuma.s4DBmDG + chDG);
                tPixel.tColour.chRed += ((int8_t)tOPLuma.s4DRmDG + chDG);
            }
            break;
        case ARM_QOI_OP_RUN: {
                __arm_qoi_op_run_t tOPRun = {
                    .tHead.chID = tChunk.chID
                };
                this.ptWorking->u6RunCount = tOPRun.u6Run;
            }
            break;
    }

    this.ptWorking->tHashTable[__arm_qoi_get_hash_index(tPixel.wValue)].wValue = tPixel.wValue;
    this.ptWorking->tPrevious.wValue = tPixel.wValue;

    ptPixel->wValue = tPixel.wValue;
    this.ptWorking->tPixelDecoded++;

    return true;
}


