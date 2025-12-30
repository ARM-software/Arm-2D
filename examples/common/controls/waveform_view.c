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
#define __GENERIC_LOADER_INHERIT__
#define __WAVEFORM_VIEW_IMPLEMENT__
#include "waveform_view.h"

#if defined(RTE_Acceleration_Arm_2D_Helper_PFB) && defined(RTE_Acceleration_Arm_2D_Extra_Loader)

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
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
ARM_NONNULL(1)
static
arm_2d_err_t __waveform_view_decoder_init(arm_generic_loader_t *ptObj);

ARM_NONNULL(1, 2, 3)
static
arm_2d_err_t __waveform_view_draw(  arm_generic_loader_t *ptObj,
                                    arm_2d_region_t *ptROI,
                                    uint8_t *pchBuffer,
                                    uint32_t iTargetStrideInByte,
                                    uint_fast8_t chBitsPerPixel);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
arm_2d_err_t waveform_view_init(waveform_view_t *ptThis,
                                waveform_view_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    memset(ptThis, 0, sizeof(waveform_view_t));

    //if (NULL != ptCFG) {
        this.tCFG = *ptCFG;
    //}

    arm_2d_err_t tResult = ARM_2D_ERR_NONE;

    do {
        if (NULL == this.tCFG.IO.ptIO) {
            this.use_as__arm_generic_loader_t.bErrorDetected = true;
            tResult = ARM_2D_ERR_IO_ERROR;
            break;
        }

        arm_generic_loader_cfg_t tCFG = {
            .bUseHeapForVRES = this.tCFG.bUseHeapForVRES,
            .tColourInfo.chScheme = ARM_2D_COLOUR,
            .bBlendWithBG = true,
            .ImageIO = {
                .ptIO = this.tCFG.IO.ptIO,
                .pTarget = this.tCFG.IO.pTarget,
            },

            .UserDecoder = {
                .fnDecoderInit = &__waveform_view_decoder_init,
                .fnDecode = &__waveform_view_draw,
            },

            .ptScene = this.tCFG.ptScene,
        };

        tResult = arm_generic_loader_init(  &this.use_as__arm_generic_loader_t,
                                            &tCFG);

        if (tResult < 0) {
            break;
        }

        this.tTile.tRegion.tSize = this.tCFG.tSize;
        if ((0 == this.tTile.tRegion.tSize.iWidth)
         || (0 == this.tTile.tRegion.tSize.iHeight)) {
            tResult = ARM_2D_ERR_INVALID_PARAM;
            break;
        }

    } while(0);

    /* update scale */
    do {
        if (this.tCFG.ChartScale.nUpperLimit == this.tCFG.ChartScale.nLowerLimit) {
            this.tCFG.ChartScale.nUpperLimit = this.tCFG.tSize.iHeight;
            this.tCFG.ChartScale.nLowerLimit = 0;
        } else if (this.tCFG.ChartScale.nUpperLimit < this.tCFG.ChartScale.nLowerLimit) {
            int32_t nTemp = this.tCFG.ChartScale.nUpperLimit;
            this.tCFG.ChartScale.nUpperLimit = this.tCFG.ChartScale.nLowerLimit;
            this.tCFG.ChartScale.nLowerLimit = nTemp;
        }

        int32_t nLength = this.tCFG.ChartScale.nUpperLimit - this.tCFG.ChartScale.nLowerLimit;
        this.q16Scale = reinterpret_q16_f32( (float)(this.tCFG.tSize.iHeight - 1) 
                                           / (float)nLength);

    } while(0);

    return tResult;

}

ARM_NONNULL(1)
void waveform_view_depose( waveform_view_t *ptThis)
{
    assert(NULL != ptThis);

    arm_generic_loader_depose(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
void waveform_view_on_load( waveform_view_t *ptThis)
{
    assert(NULL != ptThis);
    
    arm_generic_loader_on_load(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
void waveform_view_on_frame_start( waveform_view_t *ptThis)
{
    assert(NULL != ptThis);
    
    arm_generic_loader_on_frame_start(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
void waveform_view_on_frame_complete( waveform_view_t *ptThis)
{
    assert(NULL != ptThis);

    arm_generic_loader_on_frame_complete(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
static
arm_2d_err_t __waveform_view_decoder_init(arm_generic_loader_t *ptObj)
{
    assert(NULL != ptObj);

    waveform_view_t *ptThis = (waveform_view_t *)ptObj;


    return ARM_2D_ERR_NONE;
}

__STATIC_INLINE
bool __waveform_view_get_sample_y(  waveform_view_t *ptThis,
                                    int16_t *piY)
{
    //assert(NULL != ptThis);
    //assert(NULL != piY)

    uint_fast8_t chSampleDataSize = 1 << this.tCFG.u2SampleSize;

    uint32_t wData = 0;
    if (0 == arm_loader_io_read( this.tCFG.IO.ptIO, 
                        this.tCFG.IO.pTarget, 
                        &this.use_as__arm_generic_loader_t, 
                        (uint8_t *)&wData,
                        chSampleDataSize)) {
        return false;
    }
    int16_t iY = 0;

    if (this.tCFG.bUnsigned) {
        switch (this.tCFG.u2SampleSize) {
            case WAVEFORM_SAMPLE_SIZE_BYTE:
                break;
            case WAVEFORM_SAMPLE_SIZE_HWORD:
                break;
            case WAVEFORM_SAMPLE_SIZE_WORD:
                break;
            default:
                assert(false);
                break;
        }
    } else {
        switch (this.tCFG.u2SampleSize) {
            case WAVEFORM_SAMPLE_SIZE_BYTE:
                break;
            case WAVEFORM_SAMPLE_SIZE_HWORD: {
                    int16_t iData = *(int16_t *)&wData;
                    iData -= this.tCFG.ChartScale.nLowerLimit;
                    iY = (int16_t)((((int64_t)this.q16Scale * (int64_t)iData)) >> 16);
                }
                break;
            case WAVEFORM_SAMPLE_SIZE_WORD:
                break;
            default:
                assert(false);
                break;
        }
    }

    *piY = this.tCFG.tSize.iHeight - iY - 1;

    return true;
}


ARM_NONNULL(1, 2, 3)
static
arm_2d_err_t __waveform_view_draw(  arm_generic_loader_t *ptObj,
                                    arm_2d_region_t *ptROI,
                                    uint8_t *pchBuffer,
                                    uint32_t iTargetStrideInByte,
                                    uint_fast8_t chBitsPerPixel)
{
    assert(NULL != ptObj);
    assert(16 == chBitsPerPixel);

    waveform_view_t *ptThis = (waveform_view_t *)ptObj;


    int_fast16_t iXLimit = ptROI->tSize.iWidth + ptROI->tLocation.iX; 
    int_fast16_t iYLimit = ptROI->tSize.iHeight + ptROI->tLocation.iY; 
    int_fast16_t iYStart = ptROI->tLocation.iY;

    uint16_t hwBrushColour = this.tCFG.tBrushColour.hwColour;

    int16_t iX = ptROI->tLocation.iX;
    assert(iX >= 0);
    int16_t iPreviousSampleY;

    if (0 == iX) {
        arm_loader_io_seek( this.tCFG.IO.ptIO, 
                            this.tCFG.IO.pTarget, 
                            ptObj, 
                            iX, 
                            SEEK_SET);
    } else {
        arm_loader_io_seek( this.tCFG.IO.ptIO, 
                            this.tCFG.IO.pTarget, 
                            ptObj, 
                            iX - 1, 
                            SEEK_SET);
    }

    /* get previous height */
    if (!__waveform_view_get_sample_y(ptThis, &iPreviousSampleY)) {
        return ARM_2D_ERR_NONE;
    }

    arm_loader_io_seek( this.tCFG.IO.ptIO, 
                            this.tCFG.IO.pTarget, 
                            ptObj, 
                            iX, 
                            SEEK_SET);

    
    for (int16_t n = 0; n < ptROI->tSize.iWidth; n++) {

        int16_t iCurrentSampleY;

        if (!__waveform_view_get_sample_y(ptThis, &iCurrentSampleY)) {
            return ARM_2D_ERR_NONE;
        }

        if (iCurrentSampleY >= iYStart && iCurrentSampleY < iYLimit) {
            /* we should draw this point */

            int16_t iYOffset = iCurrentSampleY - iYStart;
            
            uint8_t *pchPixel = pchBuffer + iYOffset * iTargetStrideInByte;
            
            *(uint16_t *)pchPixel = hwBrushColour;
    
        }

        pchBuffer += 2;
    }

#if 0

    /* for debug only -----------------------------------------------*/
    for (int_fast16_t iY = ptROI->tLocation.iY; iY < iYLimit; iY++) {

        if ((iY & 0x0F)) {
            /* move to next line */
            pchBuffer += iTargetStrideInByte;
            continue;
        }

        uint16_t *phwPixelLine = (uint16_t *)pchBuffer;

        for (int_fast16_t iX = ptROI->tLocation.iX; iX < iXLimit; iX++) {

            if (!(iX & 0x0F)) {
                *phwPixelLine = this.tCFG.tBrushColour.hwColour;
            }

            phwPixelLine++;
        }

        /* move to next line */
        pchBuffer += iTargetStrideInByte;
    }
#endif

    return ARM_2D_ERR_NONE;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif
