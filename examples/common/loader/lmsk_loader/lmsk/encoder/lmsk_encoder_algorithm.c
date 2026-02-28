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
#if defined(__ENCODER_COMPILATION__)

#include "lmsk_encoder.h"

#include <string.h>
#include <assert.h>
#include <stddef.h>
#include <inttypes.h>

#ifdef   __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#undef this
#define this (*ptThis)

#define DEBUG_GRADIENT          0

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __arm_lmsk_algorithm_t __arm_lmsk_algorithm_t;

typedef struct __arm_lmsk_encode_result_t {
    uint16_t bHit       : 1;
    uint16_t u15Size    : 15;
    int16_t hwRawSize;
    uint8_t chNewPrevious;
    uint8_t *pchEncode;
    __arm_lmsk_algorithm_t *ptAlgorithm;
} __arm_lmsk_encode_result_t;

struct __arm_lmsk_algorithm_t {

    __arm_lmsk_encode_result_t (* const fnTry)( arm_lmsk_encoder_t *ptThis,
                                                uint8_t *pchSource, 
                                                size_t tSizeLeft, 
                                                uint8_t chPrevious,
                                                uint8_t chAlphaMSBBits);

    uint32_t wPixelCover;
    const bool bCheckPalette;
    const char * const pchName;
};

enum {
    LMSK_TAG_INDEX,
    LMSK_TAG_DELTA_LARGE,
    LMSK_TAG_REPEAT_PREVIOUS,
    LMSK_TAG_DELTA_SMALL,
    LMSK_TAG_GRADIENT,
    LMSK_TAG_ALPHA,
};


/*============================ PROTOTYPES ====================================*/

static 
__arm_lmsk_encode_result_t __arm_lmsk_try_alpha_tag(arm_lmsk_encoder_t *ptThis,
                                                    uint8_t *pchSource, 
                                                    size_t tSizeLeft, 
                                                    uint8_t chPrevious,
                                                    uint8_t chAlphaMSBBits);

static 
__arm_lmsk_encode_result_t __arm_lmsk_try_delta_large_tag(  arm_lmsk_encoder_t *ptThis,
                                                            uint8_t *pchSource, 
                                                            size_t tSizeLeft, 
                                                            uint8_t chPrevious,
                                                            uint8_t chAlphaMSBBits);

static 
__arm_lmsk_encode_result_t __arm_lmsk_try_delta_small_tag(  arm_lmsk_encoder_t *ptThis,
                                                            uint8_t *pchSource, 
                                                            size_t tSizeLeft, 
                                                            uint8_t chPrevious,
                                                            uint8_t chAlphaMSBBits);

static 
__arm_lmsk_encode_result_t __arm_lmsk_try_repeat_prev_tag(  arm_lmsk_encoder_t *ptThis,
                                                            uint8_t *pchSource, 
                                                            size_t tSizeLeft, 
                                                            uint8_t chPrevious,
                                                            uint8_t chAlphaMSBBits);

static 
__arm_lmsk_encode_result_t __arm_lmsk_try_gradient_tag( arm_lmsk_encoder_t *ptThis,
                                                        uint8_t *pchSource, 
                                                        size_t tSizeLeft, 
                                                        uint8_t chPrevious,
                                                        uint8_t chAlphaMSBBits);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static
__arm_lmsk_algorithm_t c_tAlgorithms[] = {
    [LMSK_TAG_INDEX] = {
        .fnTry = NULL,
        .pchName = "LMSK_TAG_INDEX",
    },
    [LMSK_TAG_DELTA_LARGE] = {
        .fnTry = &__arm_lmsk_try_delta_large_tag,
        .pchName = "LMSK_TAG_DELTA_LARGE",
    },
    [LMSK_TAG_REPEAT_PREVIOUS] = {
        .fnTry = &__arm_lmsk_try_repeat_prev_tag,
        .pchName = "LMSK_TAG_REPEAT_PREVIOUS",
    },
    [LMSK_TAG_DELTA_SMALL] = {
        .fnTry = &__arm_lmsk_try_delta_small_tag,
        .pchName = "LMSK_TAG_DELTA_SMALL",
    },
    [LMSK_TAG_GRADIENT] = {
        .fnTry = &__arm_lmsk_try_gradient_tag,
        .pchName = "LMSK_TAG_GRADIENT",
    },
    [LMSK_TAG_ALPHA] = {
        .fnTry = &__arm_lmsk_try_alpha_tag,
        .pchName = "LMSK_TAG_ALPHA",
        .bCheckPalette = true,
    },
};

static 
uint32_t s_wTotalPixels = 0;


/*============================ IMPLEMENTATION ================================*/

void __arm_lmsk_encoder_prepare(arm_lmsk_encoder_t *ptThis)
{
    for (uint_fast8_t n = 0; n < dimof(c_tAlgorithms); n++) {
        c_tAlgorithms[n].wPixelCover = 0;
    }
    s_wTotalPixels = 0;
}

void __arm_lmsk_encoder_report(arm_lmsk_encoder_t *ptThis)
{

    uint32_t wTotalPixels = s_wTotalPixels;

    printf("\r\n-[Report]------------------------------ \r\n");

    for (uint_fast8_t n = 0; n < dimof(c_tAlgorithms); n++) {
    
        double dfRatio = (double)c_tAlgorithms[n].wPixelCover / (double) wTotalPixels;
        dfRatio *= 100.0f;

        printf( "\r\nAlgorithm: %s \r\n", c_tAlgorithms[n].pchName);
        printf( "Pixel Hits: %"PRIu32" [%2.2f%%]\r\n", 
                c_tAlgorithms[n].wPixelCover,
                dfRatio);
    }

    printf("--------------------------------------- \r\n\r\n");
}

static inline 
__arm_lmsk_encode_result_t __arm_lmsk_encode_use_index(__arm_lmsk_encode_result_t tOld, uint8_t chIndex)
{
    __arm_lmsk_encode_result_t tResult = {
        .pchEncode = (uint8_t *)malloc(1),
        .bHit = true,
        .hwRawSize = 1,
        .u15Size = 1,
        .chNewPrevious = tOld.chNewPrevious,
        .ptAlgorithm = &c_tAlgorithms[LMSK_TAG_INDEX],
    };

    assert(NULL != tResult.pchEncode);

    tResult.pchEncode[0] = (arm_lmsk_tag_index_t) {
        .u2Tag = TAG_U2_INDEX,
        .u5Index = chIndex,
    }.chByte;

    if (NULL != tOld.pchEncode) {
        /* free the old result */
        free(tOld.pchEncode);
    }

    return tResult;
}

void __arm_lmsk_encode_line(arm_lmsk_encoder_t *ptThis,
                            __arm_lmsk_line_out_t *ptLine,
                            uint8_t chAlphaMSBBits)
{
    assert(NULL != ptThis);
    assert(NULL != ptLine);

    int16_t iWidth = this.Mask.iWidth;

    ptLine->pchBuffer = (uint8_t *)malloc(iWidth * 2); /* for the worst case*/
    assert(NULL != ptLine->pchBuffer);

    ptLine->pchBuffer[0] = ptLine->pchSourceLine[0];    /* the first pixel */

    uint8_t chPrevious = ptLine->pchBuffer[0]; 
    int16_t iSizeLeft = iWidth - 1;
    uint8_t *pchSource = ptLine->pchSourceLine + 1;
    uint8_t *pchTarget = ptLine->pchBuffer + 1;
    size_t tEncodedSize = 1;
    do {
        __arm_lmsk_encode_result_t tBestResult = {0};

        float fCompressionRate = 0.0f;

        /* try all algorithm */
        for (uint_fast8_t n = 0; n < dimof(c_tAlgorithms); n++) {

            if (NULL == c_tAlgorithms[n].fnTry) {
                continue;
            }

            if (n == LMSK_TAG_GRADIENT && this.bNoGradient) {
                continue;
            }

            __arm_lmsk_encode_result_t tResult 
                = c_tAlgorithms[n].fnTry(ptThis, pchSource, iSizeLeft, chPrevious, chAlphaMSBBits);

            if (tResult.bHit) {
                float fRate = (float)tResult.hwRawSize / (float)tResult.u15Size;
                if (    (fRate > fCompressionRate)
                   ||   (   (fRate == fCompressionRate)
                        &&  (tBestResult.hwRawSize < tResult.hwRawSize))) {
                    if (NULL != tBestResult.pchEncode) {
                        /* free the old result */
                        free(tBestResult.pchEncode);
                    }

                    fCompressionRate = fRate;
                    
                    tBestResult = tResult;
                    continue;
                } 
            }

            if (NULL != tResult.pchEncode) {
                free(tResult.pchEncode);
            }
        }

        assert(tBestResult.bHit);

        if ((tBestResult.ptAlgorithm->bCheckPalette)
        &&  (1 == tBestResult.hwRawSize)) {
            int_fast8_t chIndex = 0;
            bool bFindIndex = false;
            for (;chIndex < dimof(this.tOutput.chPalette); chIndex++) {
                if (this.tOutput.chPalette[chIndex] == tBestResult.chNewPrevious) {

                    tBestResult = __arm_lmsk_encode_use_index(tBestResult, chIndex);
                    bFindIndex = true;
                    break;
                }

                if (0 == this.tOutput.chPalette[chIndex] && chIndex > 0) {
                    break;
                }
            }

            if (!bFindIndex && chIndex < dimof(this.tOutput.chPalette)) {
                this.tOutput.chPalette[chIndex] = tBestResult.chNewPrevious;
                tBestResult = __arm_lmsk_encode_use_index(tBestResult, chIndex);
            }
        }

        /* get the best encoding decision */

        do {
            size_t tTagSize = tBestResult.u15Size;
            size_t tRawSize = tBestResult.hwRawSize;
            memcpy(pchTarget, tBestResult.pchEncode, tTagSize);
            pchTarget += tTagSize;
            pchSource += tRawSize;
            tEncodedSize += tTagSize;

            assert(iSizeLeft >= tRawSize);
            iSizeLeft -= tRawSize;

            chPrevious = tBestResult.chNewPrevious;

            if (NULL != tBestResult.ptAlgorithm) {
                tBestResult.ptAlgorithm->wPixelCover += tRawSize;
            }

            if (NULL != tBestResult.pchEncode) {
                /* free the old result */
                free(tBestResult.pchEncode);
            }

        } while(0);

    } while(iSizeLeft);

    ptLine->tSize = tEncodedSize;

    s_wTotalPixels += iWidth;
}


static 
__arm_lmsk_encode_result_t __arm_lmsk_try_alpha_tag(arm_lmsk_encoder_t *ptThis,
                                                    uint8_t *pchSource, 
                                                    size_t tSizeLeft, 
                                                    uint8_t chPrevious,
                                                    uint8_t chAlphaMSBBits)
{
    __arm_lmsk_encode_result_t tResult = {
        .pchEncode = (uint8_t *)malloc(2),
        .bHit = true,
        .hwRawSize = 1,
        .u15Size = 2,
        .ptAlgorithm = &c_tAlgorithms[LMSK_TAG_ALPHA],
    };

    assert(NULL != tResult.pchEncode);

    tResult.pchEncode[0] = TAG_U8_ALPHA;
    tResult.pchEncode[1] = *pchSource;

    tResult.chNewPrevious = *pchSource;

    return tResult;

}

static inline
int8_t __arm_lmsk_get_delta(uint16_t hwPrevious, 
                            uint16_t hwCurrent,
                            uint8_t chAlphaMSBBits)
{

    uint8_t chBitsToShift = 8 - chAlphaMSBBits;
    hwPrevious >>= chBitsToShift;
    hwCurrent >>= chBitsToShift;

    int16_t iDelta0 = (int16_t)hwCurrent - (int16_t)hwPrevious;
    uint16_t chCompenstation = 0x100 >> chBitsToShift;

    if (iDelta0 < 0) {
        int16_t iDelta1 = (int16_t)(hwCurrent + chCompenstation) - (int16_t)hwPrevious;

        if (ABS(iDelta0) < ABS(iDelta1)) {
            return iDelta0;
        } 

        return iDelta1;

    } else if (iDelta0 > 0) {
        int16_t iDelta1 = (int16_t)(hwPrevious + chCompenstation) - (int16_t)hwCurrent;

        if (ABS(iDelta0) < ABS(iDelta1)) {
            return iDelta0;
        } 

        return -iDelta1;

    }

    return iDelta0;
}

static 
__arm_lmsk_encode_result_t __arm_lmsk_try_delta_large_tag(  arm_lmsk_encoder_t *ptThis,
                                                            uint8_t *pchSource, 
                                                            size_t tSizeLeft, 
                                                            uint8_t chPrevious,
                                                            uint8_t chAlphaMSBBits)
{
    __arm_lmsk_encode_result_t tResult = {
        .pchEncode = (uint8_t *)malloc(1),
        .bHit = false,
        .hwRawSize = 1,
        .u15Size = 1,
        .ptAlgorithm = &c_tAlgorithms[LMSK_TAG_DELTA_LARGE],
    };

    assert(NULL != tResult.pchEncode);

    uint8_t chBitsToShift = 8 - chAlphaMSBBits;
    int16_t iDelta = __arm_lmsk_get_delta(  chPrevious, 
                                            pchSource[0], 
                                            chAlphaMSBBits);

    if (iDelta >= -32 && iDelta <= 31) {
        tResult.bHit = true;

        tResult.pchEncode[0] = ((arm_lmsk_tag_delta_large_t){
            .s2Tag = TAG_S2_DELTA_LARGE,
            .s6Delta = iDelta,
        }).chByte;

        tResult.chNewPrevious = (*pchSource >> chBitsToShift) << chBitsToShift;
    }

    return tResult;

}

static 
__arm_lmsk_encode_result_t __arm_lmsk_try_delta_small_tag(  arm_lmsk_encoder_t *ptThis,
                                                            uint8_t *pchSource, 
                                                            size_t tSizeLeft, 
                                                            uint8_t chPrevious,
                                                            uint8_t chAlphaMSBBits)
{
    __arm_lmsk_encode_result_t tResult = {
        .pchEncode = (uint8_t *)malloc(1),
        .bHit = false,
        .hwRawSize = 2,
        .u15Size = 1,
        .ptAlgorithm = &c_tAlgorithms[LMSK_TAG_DELTA_SMALL],
    };

    assert(NULL != tResult.pchEncode);


    uint8_t chBitsToShift = 8 - chAlphaMSBBits;
    do {
        if (tSizeLeft < 2) {
            break;
        }

        arm_lmsk_tag_delta_small_t tTagDeltaSmall = {
            .s2Tag = TAG_S2_DELTA_SMALL,
        };

        int16_t iDelta = __arm_lmsk_get_delta(  chPrevious, 
                                                pchSource[0], 
                                                chAlphaMSBBits);

        if (iDelta >= -4 && iDelta <= 3) {
            tTagDeltaSmall.s3Delta0 = iDelta;
        } else {
            break;
        }

        iDelta = __arm_lmsk_get_delta(  pchSource[0], 
                                        pchSource[1], 
                                        chAlphaMSBBits);

        if (iDelta >= -4 && iDelta <= 3) {
            tTagDeltaSmall.s3Delta1 = iDelta;
        } else {
            break;
        }

        tResult.bHit = true;
        tResult.pchEncode[0] = tTagDeltaSmall.chByte;
        tResult.chNewPrevious = (pchSource[1] >> chBitsToShift) << chBitsToShift;

    } while(0);

    return tResult;
}


static 
__arm_lmsk_encode_result_t __arm_lmsk_try_repeat_prev_tag(  arm_lmsk_encoder_t *ptThis,
                                                            uint8_t *pchSource, 
                                                            size_t tSizeLeft, 
                                                            uint8_t chPrevious,
                                                            uint8_t chAlphaMSBBits)
{
    __arm_lmsk_encode_result_t tResult = {
        .u15Size = 1,
        .ptAlgorithm = &c_tAlgorithms[LMSK_TAG_REPEAT_PREVIOUS],
    };

    size_t tRepeatCount = 0;

    uint8_t chBitsToShift = 8 - chAlphaMSBBits;
    uint8_t chShiftedPrevious = chPrevious >> chBitsToShift;

    do {
        uint8_t chCurrent = *pchSource++ >> chBitsToShift;
        if (chCurrent != chShiftedPrevious) {
            break;
        }

        tRepeatCount++;
    } while(--tSizeLeft);

    if (tRepeatCount == 0) {
        return tResult;
    }

    if (tRepeatCount <= 62) {

        tResult.bHit = true;
        tResult.hwRawSize = tRepeatCount;
        tResult.pchEncode = (uint8_t *)malloc(1);
        assert(NULL != tResult.pchEncode);

        tResult.pchEncode[0] = ((arm_lmsk_tag_repeat_t){
            .u2Tag = TAG_U2_REPETA,
            .u6Repeat = tRepeatCount - 1,
        }).chByte;

        tResult.chNewPrevious = chPrevious;
    } else {

        tResult.bHit = true;
        tResult.hwRawSize = tRepeatCount;
        tResult.pchEncode = (uint8_t *)malloc(4);
        tResult.u15Size = 4;
        assert(NULL != tResult.pchEncode);

        *(uint32_t *)tResult.pchEncode = (arm_lmsk_tag_gradient_t) {
            .chTag = TAG_U8_GRADIENT,
            .chToAlpha = chPrevious,
            .iSteps = tRepeatCount - 1,
        }.wWord;

        tResult.chNewPrevious = chPrevious;
    }

    return tResult;

}


static 
__arm_lmsk_encode_result_t __arm_lmsk_try_gradient_tag( arm_lmsk_encoder_t *ptThis,
                                                        uint8_t *pchSourceBase, 
                                                        size_t tSizeLeft, 
                                                        uint8_t chPrevious,
                                                        uint8_t chAlphaMSBBits)
{
    __arm_lmsk_encode_result_t tResult = {
        .u15Size = 4,
        .ptAlgorithm = &c_tAlgorithms[LMSK_TAG_GRADIENT],
    };

    if (tSizeLeft < 4) {
        return tResult;
    }

    uint_fast8_t chGradientTolerant = this.u2GradientTolerant;

#if DEBUG_GRADIENT
    printf("\r\n Gradient Tolerant %d\r\n", chGradientTolerant);
#endif
    uint8_t *pchSource = pchSourceBase;

    int16_t iGradientSize = 1;
    int16_t iPrevious = chPrevious;
    int16_t iCurrent = *pchSource++;
    tSizeLeft--;

#if DEBUG_GRADIENT
    printf("%02"PRIx8"\r\n", (uint8_t)iPrevious);
#endif

    uint8_t chToAlpha = iCurrent;
    int16_t iDeltaPrevious = iCurrent - iPrevious;

#if DEBUG_GRADIENT
    printf("%02"PRIx8" ", (uint8_t)iCurrent);
#endif

    iPrevious = iCurrent;
    int16_t iStepLength[2] = {2, 0};
    bool bFirstStepLength = true;

    struct {
        uint8_t chToAlpha;
        int16_t iGradientSize;
    } PreviousStep = {0};

    enum {
        STEP_LEN_CURRENT,
        STEP_LEN_PREV,
    };
    uint16_t hwStepCount = 0;
    bool bNewStep = false;
    do {
        iCurrent = *pchSource++;
        int16_t iDelta = iCurrent - iPrevious;

        int16_t iDeltaChange = iDelta - iDeltaPrevious;
    #if DEBUG_GRADIENT
        printf("%02"PRIx8" ", (uint8_t)iCurrent);
    #endif

        if (ABS(iDeltaChange) > 1) {
        #if DEBUG_GRADIENT
            printf("|");
        #endif
            break;
        }
        if (0 == iDeltaChange || bNewStep) {
            iStepLength[STEP_LEN_CURRENT]++;
            bNewStep = false;
        } else {
        #if DEBUG_GRADIENT
            printf("[%d]", iStepLength[STEP_LEN_CURRENT]);
        #endif
            if (bFirstStepLength) {
                bFirstStepLength = false;
            } else {
                
                /* compare two step length */
                int16_t iStepLengthDelta = iStepLength[STEP_LEN_CURRENT] - iStepLength[STEP_LEN_PREV];
                if (ABS(iStepLengthDelta) > chGradientTolerant) {
                    break;
                }
            }

            PreviousStep.chToAlpha = iPrevious;
            PreviousStep.iGradientSize = iGradientSize;

            hwStepCount++;

            iStepLength[STEP_LEN_PREV] = iStepLength[STEP_LEN_CURRENT];
            iStepLength[STEP_LEN_CURRENT] = 1;
            bNewStep = true;
        }


        /* wrong direction */
        if (iDelta < 0 && iDeltaPrevious > 0) {
            break;
        }
        if (iDelta > 0 && iDeltaPrevious < 0) {
            break;
        }

        iDeltaPrevious = iDelta;
        chToAlpha = iCurrent;
        iPrevious = iCurrent;

        iGradientSize++;
    } while(--tSizeLeft);

#if DEBUG_GRADIENT
    printf("\r\n Steps: %d \r\n", hwStepCount);
#endif


    if (hwStepCount > 0) {
        chToAlpha = PreviousStep.chToAlpha;
        iGradientSize = PreviousStep.iGradientSize;

    #if DEBUG_GRADIENT
        printf("\r\n Resume to %"PRIx8" Size:%d\r\n", chToAlpha, iGradientSize);
    #endif
    }

    uint8_t chStartPixel = pchSourceBase[-1];
    if (iGradientSize > 4) {

        if (chPrevious == chStartPixel) {
            /* The previous is exact the same as the original 
             * alpha.
             */
            tResult.bHit = true;
            tResult.u15Size = sizeof(arm_lmsk_tag_gradient_t);
            tResult.pchEncode = (uint8_t *)malloc(tResult.u15Size);
            assert(NULL != tResult.pchEncode);

            *(uint32_t *)tResult.pchEncode = (arm_lmsk_tag_gradient_t) {
                .chTag = TAG_U8_GRADIENT,
                .chToAlpha = chToAlpha,
                .iSteps = iGradientSize - 1,        /* must be (count - 1) */
            }.wWord;
            
            tResult.hwRawSize = iGradientSize;
            tResult.chNewPrevious = chToAlpha;
        } else if (iGradientSize > 6) {
            /* try to encode an alpha with index */
            chStartPixel = pchSourceBase[0];
            
            int_fast8_t chIndex = 0;
            bool bFindIndex = false;
            
            for (;chIndex < dimof(this.tOutput.chPalette); chIndex++) {
                if (this.tOutput.chPalette[chIndex] == chStartPixel) {
                    bFindIndex = true;
                    break;
                }

                if (0 == this.tOutput.chPalette[chIndex] && chIndex > 0) {
                    break;
                }
            }

            if (!bFindIndex && chIndex < dimof(this.tOutput.chPalette)) {
                /* we can insert an alpha to palette */
                this.tOutput.chPalette[chIndex] = chStartPixel;
                bFindIndex = true;
            }

        #if DEBUG_GRADIENT
            printf("insert a alpha %02"PRIx8"\r\n", chStartPixel);
        #endif

            if (bFindIndex) {
                /* we can insert an TAG_INDEX */
                tResult.bHit = true;
                tResult.u15Size =   sizeof(arm_lmsk_tag_gradient_t)
                                +   sizeof(arm_lmsk_tag_index_t);
                tResult.hwRawSize = iGradientSize--;
                tResult.chNewPrevious = chToAlpha;

                tResult.pchEncode = (uint8_t *)malloc(tResult.u15Size);
                assert(NULL != tResult.pchEncode);

                /* encode index */
                tResult.pchEncode[0] = (arm_lmsk_tag_index_t) {
                    .u2Tag = TAG_U2_INDEX,
                    .u5Index = chIndex,
                }.chByte;

                arm_lmsk_tag_gradient_t tTagGradient = {
                    .chTag = TAG_U8_GRADIENT,
                    .chToAlpha = chToAlpha,
                    .iSteps = iGradientSize - 1,    /* must be (count - 1) */
                };
                memcpy( &tResult.pchEncode[sizeof(arm_lmsk_tag_index_t)], 
                        &tTagGradient, 
                        sizeof(tTagGradient));

            } else {
                /* we have to insert an TAG_ALPHA */
                /* we can insert an TAG_INDEX */
                tResult.bHit = true;
                tResult.u15Size =   sizeof(arm_lmsk_tag_gradient_t)
                                +   sizeof(arm_lmsk_tag_alpha_t);
                tResult.hwRawSize = iGradientSize--;
                tResult.chNewPrevious = chToAlpha;

                tResult.pchEncode = (uint8_t *)malloc(tResult.u15Size);
                assert(NULL != tResult.pchEncode);

                /* encode index */
                *(uint16_t *)&tResult.pchEncode[0] = (arm_lmsk_tag_alpha_t) {
                    .chTag = TAG_U8_ALPHA,
                    .chAlpha = chStartPixel,
                }.hwWord;

                arm_lmsk_tag_gradient_t tTagGradient = {
                    .chTag = TAG_U8_GRADIENT,
                    .chToAlpha = chToAlpha,
                    .iSteps = iGradientSize - 1,    /* must be (count - 1) */
                };

                memcpy( &tResult.pchEncode[sizeof(arm_lmsk_tag_alpha_t)], 
                        &tTagGradient, 
                        sizeof(tTagGradient));

            }
        }
    }

#if DEBUG_GRADIENT
    if (tResult.bHit) {
        printf("From %02"PRIx8" To %02"PRIx8" In %d Steps:\r\n\r\n", chStartPixel, chToAlpha, iGradientSize);
    }
#endif
label_exit:
    return tResult;
}

#ifdef   __cplusplus
}
#endif

#endif


