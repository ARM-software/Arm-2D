/*
 * Copyright (c) 2009-2026 Arm Limited. All rights reserved.
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

#include "arm_lmsk_decoder.h"
#include <stdio.h>
#include <inttypes.h>

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#undef this
#define this (*ptThis)

#ifndef __STATIC_INLINE
#   define __STATIC_INLINE      static inline
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef MAX
#   define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#   define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

#ifndef ABS
#   define ABS(x) ((x) > 0 ? (x) : -(x))
#endif

/*============================ TYPES =========================================*/

enum {
    LMSK_STATE_DECODE_TAG = 0,
    LMSK_STATE_REPEAT,
    LMSK_STATE_GRADIENT,
};



/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#ifndef __ARM_2D_MATH_DEFINED_Q16__
#   define __ARM_2D_MATH_DEFINED_Q16__


__STATIC_INLINE 
q16_t
reinterpret_q16_s16(int16_t iIn0)
{
    return ((q16_t)(iIn0) << 16);
}

__STATIC_INLINE 
int16_t
reinterpret_s16_q16(q16_t q16In0)
{
    return (int16_t)((q16_t)(q16In0) >> 16);
}

__STATIC_INLINE 
q16_t
reinterpret_q16_f32(float fIn0)
{
    return ((q16_t)((fIn0) * 65536.0f + ((fIn0) >= 0 ? 0.5f : -0.5f)));
}

__STATIC_INLINE 
float
reinterpret_f32_q16(q16_t q16In0)
{
    return ((float)(q16In0) / 65536.0f);
}

__STATIC_INLINE 
q16_t
mul_q16(q16_t q16In0, q16_t q16In1)
{
    return (q16_t)((((int64_t)(q16In0)) * ((int64_t)(q16In1))) >> 16);
}

__STATIC_INLINE 
q16_t
mul_n_q16(q16_t q16In0, int32_t nIn1)
{
    return q16In0 * nIn1;
}

__STATIC_INLINE 
q16_t
mul_f_q16(q16_t q16In0, float fIn1)
{
    return mul_q16(q16In0, reinterpret_q16_f32(fIn1));
}

__STATIC_INLINE 
q16_t
div_q16(q16_t q16In0, q16_t q16In1)
{
    if (0 == q16In1) {
        return 0;
    }

    int64_t lTemp = ((int64_t)q16In0 << 16);
    return (q16_t)(lTemp / q16In1);
}

__STATIC_INLINE 
q16_t
div_n_q16(q16_t q16In0, int32_t nIn1)
{
    if (0 == nIn1) {
        return 0;
    }
    return (q16_t)(q16In0 / nIn1);
}

__STATIC_INLINE 
q16_t
div_f_q16(q16_t q16In0, float fIn1)
{
    return div_q16(q16In0, reinterpret_q16_f32(fIn1));
}

__STATIC_INLINE 
q16_t
abs_q16(q16_t q16In0)
{
    return ABS(q16In0);
}

#endif


__STATIC_INLINE
bool arm_lmsk_decoder_seek(arm_lmsk_decoder_t *ptThis, int32_t nOffset)
{
#if __ARM_LMSK_USE_LOADER_IO__
    return this.tCFG.IO.fnSeek(this.tCFG.IO.pTarget, nOffset);
#else
    this.nPosition = nOffset;
    return true;
#endif
}

__STATIC_INLINE
size_t arm_lmsk_decoder_read(   arm_lmsk_decoder_t *ptThis,  
                                uint8_t *pchBuffer,
                                size_t tLength)
{
#if __ARM_LMSK_USE_LOADER_IO__
    return this.tCFG.IO.fnRead(this.tCFG.IO.pTarget, pchBuffer, tLength);
#else
    memcpy(pchBuffer, &this.tCFG.pchLMSKSource[this.nPosition], tLength);
    this.nPosition += tLength;
    return tLength;
#endif
}

int arm_lmsk_decoder_init(  arm_lmsk_decoder_t *ptThis, 
                            arm_lmsk_decoder_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);

    memset(ptThis, 0, sizeof(arm_lmsk_decoder_t));

    this.tCFG = *ptCFG;

#if __ARM_LMSK_USE_LOADER_IO__
    if (NULL == this.tCFG.IO.fnSeek || NULL == this.tCFG.IO.fnRead) {
        return -1;
    }
#else
    if (NULL == this.tCFG.pchLMSKSource) {
        return -1;
    }
#endif

    /* read header and palette */
    do {
        arm_lmsk_decoder_seek(ptThis, 0);

        arm_lmsk_header_t tHeader;

        if (sizeof(tHeader) 
        !=  arm_lmsk_decoder_read(ptThis, (uint8_t *)&tHeader, sizeof(tHeader))) {
            return -1;
        }

        /* check signature */
        if (0 != memcmp(tHeader.chName, "LMSK", 4)) {
            return -1;
        }

        /* check version */
        if (tHeader.Version.chValue > ARM_LMSK_VERSION) {
            return -1;
        }

        /* to do */
        if (tHeader.tSetting.bRaw) {
            return -1;
        }

        this.tSetting = tHeader.tSetting;

        /* unsupported in this version */
        if (this.tSetting.u2TagSetBits > 0) {
            return -1;
        }

        if (sizeof(this.chPalette) 
        !=  arm_lmsk_decoder_read(ptThis, this.chPalette, sizeof(this.chPalette))) {
            return -1;
        }

    #if __ARM_LMSK_LSB_COMPENSATION_POLICY__ == 2
        if (this.tSetting.u3AlphaMSBCount < 7) {
            /* calculate LSB compenstation */

            int16_t iSteps = (1 << (this.tSetting.u3AlphaMSBCount + 1)) - 1;

            this.q16LSBCompensation = div_n_q16(reinterpret_q16_s16(255), iSteps);

        }
    #endif

        this.bValid = true;
    } while(0);


    return 0;
}

#define FLOOR_TABLE_START       (   sizeof(arm_lmsk_header_t)   \
                                +   sizeof(this.chPalette)      )

static inline
int32_t __arm_lmsk_decoder_get_line_start_postion(  arm_lmsk_decoder_t *ptThis,
                                                    int16_t iY)
{
    uint_fast8_t chFloors = this.tSetting.chFloorCount;
    if (chFloors > 0) {
        do {

            if (iY >= this.tFloorContext.iCurrent && iY < this.tFloorContext.iNext) {
                /* we are still in the same floor */
                break;
            } else if (NULL != this.tCFG.IO.fnSearchFloorContext) {
                __arm_lmsk_floor_context_t *ptContext 
                    = this.tCFG.IO.fnSearchFloorContext(this.tCFG.IO.pTarget, iY);
                if (NULL != ptContext) {
                    /* resume floor context */
                    this.tFloorContext = *ptContext;

                    if (iY >= this.tFloorContext.iCurrent && iY < this.tFloorContext.iNext) {
                        break;
                    }
                }
            }

            uint32_t wFloorStart = 0;

            bool bTopDownSearch = false;

            if (    (iY < this.tFloorContext.iCurrent)
               //&&   (this.tFloorContext.iCurrent >= 0)
               &&   (iY > (this.tFloorContext.iCurrent - iY))) {
                bTopDownSearch = true;
            } else if ( (iY > this.tFloorContext.iNext)
                     //&& (this.tFloorContext.iCurrent >= 0)
                     && (iY > (chFloors - iY))) {
                bTopDownSearch = true;
            }

            if (bTopDownSearch) {
                /* top-down search */

                uint8_t chStartFloorIndex = this.tFloorContext.chLastFloorIndex;
                uint32_t wFloorSize = 1 << (16 - this.tSetting.u2TagSetBits);
                
                wFloorStart = this.tFloorContext.wFloorStart;

                if (chStartFloorIndex == 0) {
                    chStartFloorIndex = chFloors - 1;
                    wFloorStart = (chStartFloorIndex + 1) * wFloorSize;
                }

                this.tFloorContext.iNext = this.tSetting.iHeight;

                do {
                    int16_t iFloor;

                    if (!arm_lmsk_decoder_seek(ptThis, FLOOR_TABLE_START + chStartFloorIndex * sizeof(uint16_t))) {
                        return -1;
                    }
                    if (sizeof(uint16_t) != arm_lmsk_decoder_read(ptThis, (uint8_t *)&iFloor, sizeof(uint16_t))) {
                        return -1;
                    }

                    if (iY >= iFloor) {
                        this.tFloorContext.iCurrent = iFloor;
                        this.tFloorContext.chLastFloorIndex = chStartFloorIndex;
                        break;
                    }

                    this.tFloorContext.iNext = iFloor;
                    wFloorStart -= wFloorSize;
                } while(--chStartFloorIndex);

            } else {
                /* bottom-up search */

                uint8_t chStartFloorIndex = 0;

                if (iY > this.tFloorContext.iCurrent) {
                    if (this.tFloorContext.iCurrent > 0) {
                        chStartFloorIndex = this.tFloorContext.chLastFloorIndex + 1;
                    }
                    wFloorStart = this.tFloorContext.wFloorStart;
                }

                if (!arm_lmsk_decoder_seek(ptThis, FLOOR_TABLE_START + chStartFloorIndex * sizeof(uint16_t))) {
                    return -1;
                }
                
                uint32_t wFloorSize = 1 << (16 - this.tSetting.u2TagSetBits);
                
                this.tFloorContext.iNext = this.tSetting.iHeight;

                if (chStartFloorIndex == 0) {
                    memset(&this.tFloorContext, 0, sizeof(this.tFloorContext));
                }

                for (uint_fast8_t n = chStartFloorIndex; n < chFloors; n++) {
                    int16_t iFloor;

                    if (sizeof(uint16_t) != arm_lmsk_decoder_read(ptThis, (uint8_t *)&iFloor, sizeof(uint16_t))) {
                        return -1;
                    }

                    if (iFloor <= iY) {
                        wFloorStart += wFloorSize;
                        this.tFloorContext.iCurrent = iFloor;
                        this.tFloorContext.chLastFloorIndex = n;
                    }

                    if (iFloor > iY) {
                        this.tFloorContext.iNext = iFloor;
                        break;
                    }
                }
            }

            this.tFloorContext.wFloorStart = wFloorStart;
        } while(0);

        if (NULL != this.tCFG.IO.fnReportFloorContext) {
            (*this.tCFG.IO.fnReportFloorContext)(this.tCFG.IO.pTarget, &this.tFloorContext);
        }
    }

    /* get to line index */
    if (!arm_lmsk_decoder_seek( ptThis, 
                                FLOOR_TABLE_START 
                                + (this.tSetting.chFloorCount + iY) * sizeof(uint16_t))) {
        return -1;
    }

    /* read line offset */
    uint32_t wLineStart = 0;
    if (sizeof(uint16_t) != arm_lmsk_decoder_read(ptThis, (uint8_t *)&wLineStart, sizeof(uint16_t))) {
        return -1;
    }

    return wLineStart + this.tFloorContext.wFloorStart;
}

__attribute__((always_inline))
static uint8_t __arm_lsmk_decode_fetch_byte(arm_lmsk_decoder_t *ptThis)
{
    uint8_t chByte;
    if (0 == this.chTagFetchByteLeft) {

    #if __ARM_LMSK_USE_LOADER_IO__
        size_t tActualRead =
        arm_lmsk_decoder_read(  ptThis,
                                (uint8_t *)&this.wTagFetchBuffer,
                                sizeof(this.wTagFetchBuffer));
        assert(tActualRead == sizeof(this.wTagFetchBuffer));
        this.chTagFetchByteLeft = sizeof(this.wTagFetchBuffer);
    #else
        /* guaranteed word access */
        assert(0 == (this.nPosition & 0x03));
        this.wTagFetchBuffer = *(uint32_t *)&this.tCFG.pchLMSKSource[this.nPosition];
        this.nPosition += 4;
        this.chTagFetchByteLeft = sizeof(this.wTagFetchBuffer);
    #endif
    }                                   
                                
    this.chTagFetchByteLeft--;
    chByte = this.wTagFetchBuffer & 0xFF;
    this.wTagFetchBuffer >>= 8;                                              
    
    return chByte;

}

static inline 
uint8_t __arm_lmsk_update_with_delta(   arm_lmsk_decoder_t *ptThis,
                                        uint8_t chCurrent, 
                                        int_fast8_t chDelta)
{
    int16_t iCurrent = (uint8_t)chCurrent;
    uint8_t chBitsToIgnore = 7 - this.tSetting.u3AlphaMSBCount;
    if (chBitsToIgnore == 0) {
        iCurrent += chDelta;
        chCurrent = ((uint8_t *)&iCurrent)[0];
    } else {
        iCurrent >>= chBitsToIgnore;
        iCurrent += chDelta;
        iCurrent <<= chBitsToIgnore;
        chCurrent = ((uint8_t *)&iCurrent)[0] & 0xFF;
    }

    return chCurrent;
}

/*!
 * \brief try to fetch next alpha
 * \param[in] ptThis the decoder
 * \param[in] pchBuffer a buffer to hold the pixel alpha
 * \param[in] iTryToSkip a number of pixels the decoder try to ignore
 * \return int16_t advance in the line 
 */
static inline 
int16_t __arm_lmsk_get_next_alpha(  arm_lmsk_decoder_t *ptThis, 
                                    uint8_t *pchBuffer, 
                                    int16_t iTryToSkip)
{
    uint8_t chCurrent = this.chPrevious;
    int16_t iAdvance = 1;

    switch (this.chState) {
        case LMSK_STATE_DECODE_TAG: 
            (void)chCurrent;

            uint8_t chTag = __arm_lsmk_decode_fetch_byte(ptThis);
            
            if (chTag == TAG_U8_ALPHA) {
                chCurrent = __arm_lsmk_decode_fetch_byte(ptThis);
            } else if (chTag == TAG_U8_GRADIENT) {
                union {
                    uint8_t chBuff[sizeof(arm_lmsk_tag_gradient_t)];
                    arm_lmsk_tag_gradient_t tTag;
                } Gradient;

                uint8_t *pchBuff = Gradient.chBuff;
                *pchBuff++ = chTag;
                *pchBuff++ = __arm_lsmk_decode_fetch_byte(ptThis);
                *pchBuff++ = __arm_lsmk_decode_fetch_byte(ptThis);
                *pchBuff++ = __arm_lsmk_decode_fetch_byte(ptThis);

                this.iGradientSteps = Gradient.tTag.iSteps + 1;
                this.chGradientFromAlpha = chCurrent;
                this.chGradientToAlpha = Gradient.tTag.chToAlpha;

                if (this.chGradientToAlpha == this.chGradientFromAlpha) {
                    int16_t iRepeat = this.iGradientSteps;

                    if (iTryToSkip) {
                        iTryToSkip = MIN(iTryToSkip, iRepeat);
                        iAdvance = iTryToSkip;
                        iRepeat -= iTryToSkip;
                    } else {
                        iRepeat--;
                    }

                    if (iRepeat > 0) {
                        this.u15Repeat = iRepeat;

                        /* goto pixel repeat state */
                        this.chState = LMSK_STATE_REPEAT;
                    }
                } else {
                    this.q16Gradient = div_n_q16(
                        reinterpret_q16_s16(    (int16_t)Gradient.tTag.chToAlpha 
                                        -    (int16_t)chCurrent),
                        this.iGradientSteps);
                    this.iGradientStepIndex = 1;

                    if (this.iGradientStepIndex == this.iGradientSteps) {
                        chCurrent = this.chGradientToAlpha;

                        /* go back to tag decoding */
                        //this.chState = LMSK_STATE_DECODE_TAG;
                    } else {
                        chCurrent = this.chGradientFromAlpha
                                + reinterpret_s16_q16( mul_n_q16(   this.q16Gradient, 
                                                                    this.iGradientStepIndex));

                        this.chState = LMSK_STATE_GRADIENT;
                    }
                }
            } else switch (chTag & 0x03) {
                case TAG_U2_INDEX: {
                        arm_lmsk_tag_index_t tTagIndex = *(arm_lmsk_tag_index_t *)&chTag;
                        if (tTagIndex.bDoWhile) {
                            /* do while: todo  */
                            assert(false);
                        } else {
                            chCurrent = this.chPalette[tTagIndex.u5Index];
                        }
                    }
                    break;
            
                case TAG_U2_REPETA: {
                        arm_lmsk_tag_repeat_t tTagRepet = *(arm_lmsk_tag_repeat_t *)&chTag;
                        /* NOTE: Repeat count = u6Repeat + 1 */
                        int16_t iRepeat = tTagRepet.u6Repeat + 1;

                        if (iTryToSkip) {
                            iTryToSkip = MIN(iTryToSkip, iRepeat);
                            iAdvance = iTryToSkip;
                            iRepeat -= iTryToSkip;
                        } else {
                            iRepeat--;
                        }

                        if (iRepeat > 0) {
                            this.u15Repeat = iRepeat;

                            /* goto pixel repeat state */
                            this.chState = LMSK_STATE_REPEAT;
                        }
                    }
                    break;
                    
                case TAG_U2_DELTA_SMALL:{
                        arm_lmsk_tag_delta_small_t tTagDeltaSmall
                            = *(arm_lmsk_tag_delta_small_t *)&chTag;

                        chCurrent = __arm_lmsk_update_with_delta(   
                                                        ptThis, 
                                                        chCurrent, 
                                                        tTagDeltaSmall.s3Delta0);

                        /* use repeat to return the next pixel */
                        this.chPrevious = __arm_lmsk_update_with_delta(   
                                                        ptThis, 
                                                        chCurrent, 
                                                        tTagDeltaSmall.s3Delta1);

                        if (iTryToSkip >= 2) {
                            iAdvance = 2;
                        } else {
                            this.u15Repeat = 1;
                            /* goto pixel repeat state */
                            this.chState = LMSK_STATE_REPEAT;
                        }

                        goto label_exit;
                    }

                case TAG_U2_DELTA_LARGE: {
                        arm_lmsk_tag_delta_large_t tTagDeltaLarge 
                            = *(arm_lmsk_tag_delta_large_t *)&chTag;

                        chCurrent = __arm_lmsk_update_with_delta(   
                                                        ptThis, 
                                                        chCurrent, 
                                                        tTagDeltaLarge.s6Delta);      
                    }
                    break;
            }

            break;
        case LMSK_STATE_REPEAT:     /* repeat pixels */
            assert(this.u15Repeat);
            if (iTryToSkip) {
                iAdvance = MIN(iTryToSkip, this.u15Repeat);
                this.u15Repeat -= iAdvance;
            } else {
                this.u15Repeat--;
            }
            if (0 == this.u15Repeat) {
                /* go back to tag decoding */
                this.chState = LMSK_STATE_DECODE_TAG;
            }
            break;
        case LMSK_STATE_GRADIENT:
            if (iTryToSkip) {
                int16_t iStepLeft = this.iGradientSteps - this.iGradientStepIndex;
                iAdvance = MIN(iTryToSkip, iStepLeft);
                this.iGradientStepIndex += iAdvance;
            } else {
                this.iGradientStepIndex++;
            }

            if (this.iGradientStepIndex == this.iGradientSteps) {
                chCurrent = this.chGradientToAlpha;

                /* go back to tag decoding */
                this.chState = LMSK_STATE_DECODE_TAG;
            } else {
                chCurrent = this.chGradientFromAlpha
                          + reinterpret_s16_q16( mul_n_q16( this.q16Gradient, 
                                                            this.iGradientStepIndex));
            }
            break;
    }

    this.chPrevious = chCurrent;

label_exit:
    *pchBuffer = chCurrent;

    return iAdvance;
}

static
int __arm_lmsk_decode_line( arm_lmsk_decoder_t *ptThis, 
                            uint32_t wLineStartPosition,
                            int16_t iXStart,
                            int16_t iWidth,
                            uint8_t *pchBuffer)
{

    int16_t iXLimit = iXStart + iWidth - 1;

    /* first fetch */
    do {
        /* move to 32bit aligned address */
        if (!arm_lmsk_decoder_seek(ptThis, wLineStartPosition & ~0x3)) {
            return -1;
        }

        /* load first word */
        if (sizeof(this.wTagFetchBuffer) != arm_lmsk_decoder_read(
                                        ptThis, 
                                        (uint8_t *)&this.wTagFetchBuffer, 
                                        sizeof(this.wTagFetchBuffer))) {
            return -1;
        }

        if (wLineStartPosition & 0x3) {
            /* unaligned */
            this.chTagFetchByteLeft = 4 - (wLineStartPosition & 0x3);
            this.wTagFetchBuffer >>= (wLineStartPosition & 0x3) * 8;
        } else {
            this.chTagFetchByteLeft = 4;
        }
    } while(0);

    this.chState = LMSK_STATE_DECODE_TAG;

    /* first pixel */
    this.chPrevious = this.wTagFetchBuffer & 0xFF;
    this.wTagFetchBuffer >>= 8;
    this.chTagFetchByteLeft--;


    int16_t iX = 0;
    if (iX >= iXStart && iX <= iXLimit) {
        *pchBuffer++ = this.chPrevious;
        if (0 == --iWidth) {
            return 0;
        }
    }
    iX++;

    /* try to skip pixels */
    do {
        if (iX >= iXStart) {
            break;
        }

        uint8_t chCurrent;
        int16_t iPixelsBeforeWindow = iXStart - iX;

        iX += __arm_lmsk_get_next_alpha(ptThis, &chCurrent, iPixelsBeforeWindow);

        if (iX >= iXStart) {
            break;
        }

    } while(iX <= iXLimit);


    if (this.tSetting.u3AlphaMSBCount == 7) {
        do {
            uint8_t chCurrent;
            __arm_lmsk_get_next_alpha(ptThis, pchBuffer++, 0);
            iX++;
        } while(iX <= iXLimit);

    } else {
        do {
            uint8_t chCurrent;
            __arm_lmsk_get_next_alpha(ptThis, &chCurrent, 0);

            uint8_t chBitsToIgnore = 7 - this.tSetting.u3AlphaMSBCount;
            uint16_t iCurrent = chCurrent;
            iCurrent >>= chBitsToIgnore;

        #if __ARM_LMSK_LSB_COMPENSATION_POLICY__  == 2
            /* implement LSB compenstation */
            //iCurrent <<= chBitsToIgnore;
            uint8_t chMask = ((1 << (8 - chBitsToIgnore)) - 1);
            chCurrent = ((uint8_t *)&iCurrent)[0] & chMask;
            
            if (chCurrent == chMask) {
                chCurrent = 0xFF;
            } else {
                chCurrent = 
                    reinterpret_s16_q16(
                        mul_n_q16(this.q16LSBCompensation,
                                    chCurrent)); 
            }
        #elif __ARM_LMSK_LSB_COMPENSATION_POLICY__ == 1
            uint8_t chMask = ((1 << (8 - chBitsToIgnore)) - 1);
            chCurrent = ((uint8_t *)&iCurrent)[0] & chMask;
            
            if (chCurrent == chMask) {
                chCurrent = 0xFF;
            } else {
                chCurrent <<= chBitsToIgnore;
            }
        #else
            iCurrent <<= chBitsToIgnore;
            chCurrent = ((uint8_t *)&iCurrent)[0] & 0xFF;
            
        #endif

            *pchBuffer++ = chCurrent;

            iX++;
        } while(iX <= iXLimit);
    }

    return 0;
}


int arm_lmsk_decode(arm_lmsk_decoder_t *ptThis,
                    int16_t iX,
                    int16_t iY,
                    int16_t iWidth,
                    int16_t iHeight,
                    uint8_t *pchBuffer,
                    uint32_t wTargetStride)
{
    if (!this.bValid) {
        return -1;
    }

    assert(iX < this.tSetting.iWidth);
    assert(iY < this.tSetting.iHeight);

    uint32_t wDataSectionStart 
        = sizeof(arm_lmsk_header_t)                     /* header */
        + sizeof(this.chPalette)                        /* palette */
        + this.tSetting.chFloorCount * sizeof(uint16_t) /* floor table */
        + this.tSetting.iHeight * sizeof(uint16_t);     /* line index table */

    int16_t iYLimit = iY + iHeight - 1;
    

    for (; iY <= iYLimit; iY++) {

        int32_t iLineOffset = __arm_lmsk_decoder_get_line_start_postion(ptThis, iY);
        if (iLineOffset < 0) {
            return -1;
        }

        uint32_t wLinePosition = wDataSectionStart + iLineOffset;

        if (0 != __arm_lmsk_decode_line(ptThis, wLinePosition, iX, iWidth, pchBuffer)) {
            return -1;
        }

        pchBuffer += wTargetStride;
    }
    return 0;
}


#ifdef   __cplusplus
}
#endif



