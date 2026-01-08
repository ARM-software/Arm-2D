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

#include "__arm_2d_impl.h"

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
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

/*============================ MACROS ========================================*/

#undef __ARM_2D_PIXEL_BLENDING

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define __ARM_2D_PIXEL_BLENDING         __ARM_2D_PIXEL_BLENDING_GRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define __ARM_2D_PIXEL_BLENDING         __ARM_2D_PIXEL_BLENDING_RGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define __ARM_2D_PIXEL_BLENDING         __ARM_2D_PIXEL_BLENDING_CCCN888

#else
#   error Unsupported colour depth!
#endif

#undef this
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
enum {
    WAVEFORM_START = 0,
    WAVEFORM_UPDATE_BINS,
    WAVEFORM_DONE,
};

enum {
    WAVEFORM_BIN_LAST = 0,
    WAVEFORM_BIN_NEW,
};

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

__STATIC_INLINE
bool __waveform_view_get_sample_y(  waveform_view_t *ptThis,
                                    q16_t *pq16Y);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
arm_2d_err_t waveform_view_init(waveform_view_t *ptThis,
                                waveform_view_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    memset(ptThis, 0, sizeof(waveform_view_t));

    this.tBrushColour = ptCFG->tBrushColour;
    this.tSize = ptCFG->tSize;
    this.wSettings = ptCFG->wSettings;

    this.__bValid = false;

    arm_2d_err_t tResult = ARM_2D_ERR_NONE;

    do {
        if (NULL == ptCFG->IO.ptIO) {
            this.use_as__arm_generic_loader_t.bErrorDetected = true;
            tResult = ARM_2D_ERR_IO_ERROR;
            break;
        }

        arm_generic_loader_cfg_t tCFG = {
            .bUseHeapForVRES = ptCFG->bUseHeapForVRES,
            .tColourInfo.chScheme = ARM_2D_COLOUR,
            .bBlendWithBG = true,
            .tBackgroundColour = ptCFG->tBackgroundColour,

            .ImageIO = {
                .ptIO = ptCFG->IO.ptIO,
                .pTarget = ptCFG->IO.pTarget,
            },

            .UserDecoder = {
                .fnDecoderInit = &__waveform_view_decoder_init,
                .fnDecode = &__waveform_view_draw,
            },

            .ptScene = ptCFG->ptScene,
        };

        tResult = arm_generic_loader_init(  &this.use_as__arm_generic_loader_t,
                                            &tCFG);

        if (tResult < 0) {
            break;
        }

        this.tTile.tRegion.tSize = this.tSize;
        if ((0 == this.tTile.tRegion.tSize.iWidth)
         || (0 == this.tTile.tRegion.tSize.iHeight)) {
            tResult = ARM_2D_ERR_INVALID_PARAM;
            break;
        }

    } while(0);

    /* update scale */
    do {

        this.ChartScale.nUpperLimit = ptCFG->ChartScale.nUpperLimit;
        this.ChartScale.nLowerLimit = ptCFG->ChartScale.nLowerLimit;

        if (this.ChartScale.nUpperLimit == this.ChartScale.nLowerLimit) {
            this.ChartScale.nUpperLimit = this.tSize.iHeight;
            this.ChartScale.nLowerLimit = 0;
        } else if (this.ChartScale.nUpperLimit < this.ChartScale.nLowerLimit) {
            int32_t nTemp = this.ChartScale.nUpperLimit;
            this.ChartScale.nUpperLimit = this.ChartScale.nLowerLimit;
            this.ChartScale.nLowerLimit = nTemp;
        }

        this.iDiagramHeight = this.tSize.iHeight - 1 - this.u5DotHeight;
        this.iStartYOffset = (this.u5DotHeight + 1) >> 1;

        int32_t nLength = this.ChartScale.nUpperLimit - this.ChartScale.nLowerLimit;
        this.q16Scale = reinterpret_q16_f32( (float)this.iDiagramHeight 
                                           / (float)nLength);


    } while(0);

    this.ptDirtyBins = ptCFG->ptDirtyBins;
    if (NULL == this.use_as__arm_generic_loader_t.tCFG.ptScene) {
        this.bUseDirtyRegion = false;
    } else if (this.bUseDirtyRegion) {
        arm_2d_scene_player_dynamic_dirty_region_init(  
                                            &this.DirtyRegion.tDirtyRegionItem,
                                            this.use_as__arm_generic_loader_t.tCFG.ptScene);

        if (this.chDirtyRegionItemCount > 0 && NULL != this.ptDirtyBins) {

            /* update bin width */
            this.DirtyRegion.q16BinWidth 
                = reinterpret_q16_f32(  (float)this.tSize.iWidth 
                                     /  (float)this.chDirtyRegionItemCount);

            /* reset bins */
            memset( this.ptDirtyBins, 
                    0, 
                    (   sizeof(waveform_view_dirty_bin_t) 
                    *   this.chDirtyRegionItemCount));
            
            if (255 == this.chDirtyRegionItemCount) {
                this.chDirtyRegionItemCount = 254;
            }
        } else {
            this.chDirtyRegionItemCount = 0;
        }
    }

    if (this.bShowShadow) {

        this.Shadow.tColour = ptCFG->Shadow.tColour;
        this.Shadow.tGradient = ptCFG->Shadow.tGradient;

        uint32_t wTemp = *(uint32_t *)&this.Shadow.tGradient;
        this.__bShowGradient = ((0 != wTemp) && (0xFFFFFFFF != wTemp));

        if (this.__bShowGradient) {

            /* generate opacity ratio */
            int16_t iOpacityDelta = this.Shadow.tGradient.chTopRight
                                  - this.Shadow.tGradient.chTopLeft;
            int16_t iWidth = this.tSize.iWidth;
            this.Shadow.q16UpperGradientRatio = 
                div_n_q16(reinterpret_q16_s16(iOpacityDelta), iWidth);

            iOpacityDelta = this.Shadow.tGradient.chBottomRight
                          - this.Shadow.tGradient.chBottomLeft;
            
            this.Shadow.q16LowerGradientRatio = 
                div_n_q16(reinterpret_q16_s16(iOpacityDelta), iWidth);
        }
    } else {
        this.__bShowGradient = false;
    }

    this.__bValid = true;
    
    return tResult;

}

ARM_NONNULL(1)
void waveform_view_depose( waveform_view_t *ptThis)
{
    assert(NULL != ptThis);

    if (this.bUseDirtyRegion) {
        arm_2d_scene_player_dynamic_dirty_region_depose(
                                            &this.DirtyRegion.tDirtyRegionItem,
                                            this.use_as__arm_generic_loader_t.tCFG.ptScene);
    }

    arm_generic_loader_depose(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1)
arm_2d_err_t waveform_view_update_chart_scale(  waveform_view_t *ptThis,
                                                int32_t nUpperLimie, 
                                                int32_t nLowerLimit)
{
    assert(NULL != ptThis);

    if (!this.__bValid) {
        return ARM_2D_ERR_INVALID_STATUS;
    }
    if (nUpperLimie <= nLowerLimit) {
        return ARM_2D_ERR_INVALID_PARAM;
    }

    arm_irq_safe {
        this.ChartScale.nUpperLimit = nUpperLimie;
        this.ChartScale.nLowerLimit = nLowerLimit;

        int32_t nLength = nUpperLimie - nLowerLimit;
        this.q16Scale = reinterpret_q16_f32( (float)this.iDiagramHeight 
                                            / (float)nLength);

        /* request a full update */
        this.__bFullUpdateReq = true;
    }

    return ARM_2D_ERR_NONE;
}

ARM_NONNULL(1)
void waveform_view_on_load( waveform_view_t *ptThis)
{
    assert(NULL != ptThis);
    if (!this.__bValid) {
        return ;
    }
    
    arm_generic_loader_on_load(&this.use_as__arm_generic_loader_t);
}

static void __waveform_view_update_dirty_bins(waveform_view_t *ptThis)
{
    assert(NULL != ptThis);
    assert(NULL != this.ptDirtyBins);
    assert(this.chDirtyRegionItemCount > 0);
    
    uint_fast8_t chSampleDataSize = 1 << this.u2SampleSize;
    int16_t iLastUnUpdatedX = 0;
    uint_fast8_t nDotHeight = this.u5DotHeight + 1; 

    for (   uint_fast8_t chBinIndex = 0; 
            chBinIndex < this.chDirtyRegionItemCount; 
            chBinIndex++) {

        waveform_view_dirty_bin_t *ptBin = &this.ptDirtyBins[chBinIndex];

        /* save the last dirty bin */
        ptBin->Coverage[WAVEFORM_BIN_LAST] = ptBin->Coverage[WAVEFORM_BIN_NEW];
        
        memset( &ptBin->Coverage[WAVEFORM_BIN_NEW], 
                0,
                sizeof(ptBin->Coverage[WAVEFORM_BIN_NEW]));

        /* calculate the new dirty bin */
        int16_t iX1 =reinterpret_s16_q16( mul_n_q16(this.DirtyRegion.q16BinWidth,
                                                    chBinIndex + 1));

        int16_t iSampleCount = iX1 - iLastUnUpdatedX;
        if (iSampleCount <= 0) {
            continue;
        }

        if (iLastUnUpdatedX == 0) {
            arm_loader_io_seek( this.use_as__arm_generic_loader_t.tCFG.ImageIO.ptIO, 
                                this.use_as__arm_generic_loader_t.tCFG.ImageIO.pTarget, 
                                &this.use_as__arm_generic_loader_t, 
                                0, 
                                SEEK_SET);
        } else {
            arm_loader_io_seek( this.use_as__arm_generic_loader_t.tCFG.ImageIO.ptIO, 
                                this.use_as__arm_generic_loader_t.tCFG.ImageIO.pTarget, 
                                &this.use_as__arm_generic_loader_t, 
                                (iLastUnUpdatedX - 1) * chSampleDataSize, 
                                SEEK_SET);
        }

        q16_t q16LastY = 0;
        if (!__waveform_view_get_sample_y(  ptThis, &q16LastY)) {
            continue;
        }

        arm_loader_io_seek( this.use_as__arm_generic_loader_t.tCFG.ImageIO.ptIO, 
                            this.use_as__arm_generic_loader_t.tCFG.ImageIO.pTarget, 
                            &this.use_as__arm_generic_loader_t, 
                            iLastUnUpdatedX * chSampleDataSize, 
                            SEEK_SET);

        iLastUnUpdatedX = iX1;
        ptBin->iWidth = iSampleCount++;
    
        q16_t q16Ymin = __Q16_MAX__;
        q16_t q16Ymax = 0;

        q16LastY -= reinterpret_q16_s16(nDotHeight);
        q16Ymin = MIN(q16LastY, q16Ymin);
        q16Ymax = MAX(q16LastY, q16Ymax);

        do {
            q16_t q16Y;
            if (!__waveform_view_get_sample_y(  ptThis, &q16Y)) {
                break;
            }

            q16Ymin = MIN(q16Y, q16Ymin);
            q16Ymax = MAX(q16Y, q16Ymax);
        } while(--iSampleCount);

        ptBin->Coverage[WAVEFORM_BIN_NEW].iY0 = reinterpret_s16_q16(q16Ymin) - nDotHeight;
        ptBin->Coverage[WAVEFORM_BIN_NEW].iY1 = reinterpret_s16_q16(q16Ymax) + 1 + nDotHeight;
    }
}

ARM_NONNULL(1)
void waveform_view_on_frame_start( waveform_view_t *ptThis, bool bUpdate)
{
    assert(NULL != ptThis);

    if (!this.__bValid) {
        return ;
    }

    arm_generic_loader_on_frame_start(&this.use_as__arm_generic_loader_t);

    if (this.bUseDirtyRegion && bUpdate) {

        __waveform_view_update_dirty_bins(ptThis);

        arm_2d_dynamic_dirty_region_on_frame_start(
                                            &this.DirtyRegion.tDirtyRegionItem,
                                            WAVEFORM_START);
    }
}

ARM_NONNULL(1)
void waveform_view_on_frame_complete( waveform_view_t *ptThis)
{
    assert(NULL != ptThis);
    if (!this.__bValid) {
        return ;
    }

    arm_generic_loader_on_frame_complete(&this.use_as__arm_generic_loader_t);
}

ARM_NONNULL(1,2)
void waveform_view_show(waveform_view_t *ptThis,
                        const arm_2d_tile_t *ptTile,
                        const arm_2d_region_t *ptRegion,
                        bool bIsNewFrame)
{
    ARM_2D_UNUSED(bIsNewFrame);

    assert(NULL!= ptThis);

    if (!this.__bValid) {
        return ;
    }

    if (-1 == (intptr_t)ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
    }

    arm_2d_container(ptTile, __waveform_panel, ptRegion) {

        arm_2d_align_centre(__waveform_panel_canvas, 
                            this.tSize) {
            arm_2d_tile_copy_only(  &this.tTile, 
                                    &__waveform_panel, 
                                    &__centre_region);

            if (this.bUseDirtyRegion) {
                /* update dirty region */
                switch (arm_2d_dynamic_dirty_region_wait_next(
                            &this.DirtyRegion.tDirtyRegionItem)) {
                    case WAVEFORM_START:
                        if ( 0 == this.chDirtyRegionItemCount || this.__bFullUpdateReq) {
                            this.__bFullUpdateReq = false;
                            /* The user doesn't provide dirty region bins, let's update the whole diagram instead */
                            arm_2d_dynamic_dirty_region_update(
                                    &this.DirtyRegion.tDirtyRegionItem,
                                    &__waveform_panel,
                                    &__centre_region,
                                    WAVEFORM_DONE);
                            break;
                        } else {
                            this.DirtyRegion.chCurrentBin = 0;
                        }
                        //fall-through
                    case WAVEFORM_UPDATE_BINS: {

                            do {
                                uint8_t chBinIndex = this.DirtyRegion.chCurrentBin;
                                int16_t iX =reinterpret_s16_q16( 
                                                mul_n_q16(  this.DirtyRegion.q16BinWidth,
                                                            chBinIndex));
                                waveform_view_dirty_bin_t *ptBin = &this.ptDirtyBins[chBinIndex];

                                int16_t iY0 = ptBin->Coverage[1].iY0;
                                int16_t iY1 = ptBin->Coverage[1].iY1;

                                if (ptBin->Coverage[0].iY0 != ptBin->Coverage[0].iY1) {
                                    iY0 = MIN(iY0, ptBin->Coverage[0].iY0);
                                    iY1 = MAX(iY1, ptBin->Coverage[0].iY1);
                                }

                                arm_2d_region_t tDrawRegion = {
                                    .tLocation = {
                                        .iX = __centre_region.tLocation.iX + iX,
                                        .iY = __centre_region.tLocation.iY + iY0,
                                    },
                                    .tSize = {
                                        .iWidth = ptBin->iWidth,
                                        .iHeight = iY1 - iY0,
                                    },
                                };
                                if (!arm_2d_region_intersect(&tDrawRegion, &__centre_region, NULL)) {
                                    this.DirtyRegion.chCurrentBin++;
                                    if (this.DirtyRegion.chCurrentBin >= this.chDirtyRegionItemCount) {
                                        /* end early*/
                                        arm_2d_dynamic_dirty_region_change_user_region_index_only(
                                            &this.DirtyRegion.tDirtyRegionItem,
                                            WAVEFORM_DONE
                                        );
                                    } else {
                                        continue;
                                    }
                                    break;
                                }

                                if (this.DirtyRegion.chCurrentBin++ < this.chDirtyRegionItemCount) {
                                    arm_2d_dynamic_dirty_region_update(
                                                &this.DirtyRegion.tDirtyRegionItem,
                                                &__waveform_panel,
                                                &tDrawRegion,
                                                WAVEFORM_UPDATE_BINS);
                                    break;
                                } else {
                                    arm_2d_dynamic_dirty_region_update(
                                                &this.DirtyRegion.tDirtyRegionItem,
                                                &__waveform_panel,
                                                &tDrawRegion,
                                                WAVEFORM_DONE);
                                    break;
                                }
                            } while(true);
                        }
                        break;
                    default:
                    case WAVEFORM_DONE:
                        break;
                }
            }
        }
    }
}

ARM_NONNULL(1)
static
arm_2d_err_t __waveform_view_decoder_init(arm_generic_loader_t *ptObj)
{
    assert(NULL != ptObj);

    waveform_view_t *ptThis = (waveform_view_t *)ptObj;
    ARM_2D_UNUSED(ptThis);


    return ARM_2D_ERR_NONE;
}

__STATIC_INLINE
bool __waveform_view_get_sample_y(  waveform_view_t *ptThis,
                                    q16_t *pq16Y)
{
    //assert(NULL != ptThis);
    //assert(NULL != piY)

    uint_fast8_t chSampleDataSize = 1 << this.u2SampleSize;

    uint32_t wData = 0;
    if (0 == arm_loader_io_read( this.use_as__arm_generic_loader_t.tCFG.ImageIO.ptIO, 
                        this.use_as__arm_generic_loader_t.tCFG.ImageIO.pTarget, 
                        &this.use_as__arm_generic_loader_t, 
                        (uint8_t *)&wData,
                        chSampleDataSize)) {
        return false;
    }
    q16_t q16Y = 0;

    if (this.bUnsigned) {
        switch (this.u2SampleSize) {
            case WAVEFORM_SAMPLE_SIZE_BYTE: {
                    uint8_t chData = *(uint8_t *)&wData;
                    chData -= this.ChartScale.nLowerLimit;
                    q16Y = ((((int64_t)this.q16Scale * (int64_t)chData)));
                }
                break;
            case WAVEFORM_SAMPLE_SIZE_HWORD: {
                    uint16_t hwData = *(uint16_t *)&wData;
                    hwData -= this.ChartScale.nLowerLimit;
                    q16Y = ((((int64_t)this.q16Scale * (int64_t)hwData)));
                }
                break;
            case WAVEFORM_SAMPLE_SIZE_WORD: {
                    wData -= this.ChartScale.nLowerLimit;
                    q16Y = ((((int64_t)this.q16Scale * (int64_t)wData)));
                }
                break;
            default:
                assert(false);
                break;
        }
    } else {
        switch (this.u2SampleSize) {
            case WAVEFORM_SAMPLE_SIZE_BYTE: {
                    int8_t chData = *(int8_t *)&wData;
                    chData -= this.ChartScale.nLowerLimit;
                    q16Y = ((((int64_t)this.q16Scale * (int64_t)chData)));
                }
                break;
            case WAVEFORM_SAMPLE_SIZE_HWORD: {
                    int16_t iData = *(int16_t *)&wData;
                    iData -= this.ChartScale.nLowerLimit;
                    q16Y = ((((int64_t)this.q16Scale * (int64_t)iData)));
                }
                break;
            case WAVEFORM_SAMPLE_SIZE_WORD: {
                    int32_t nData = *(int32_t *)&wData;
                    nData -= this.ChartScale.nLowerLimit;
                    q16Y = ((((int64_t)this.q16Scale * (int64_t)nData)));
                }
                break;
            default:
                assert(false);
                break;
        }
    }

    *pq16Y = reinterpret_q16_s16(this.iStartYOffset + this.iDiagramHeight) - q16Y;

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
    assert(__GLCD_CFG_COLOUR_DEPTH__ == chBitsPerPixel);
    waveform_view_t *ptThis = (waveform_view_t *)ptObj;

    if (!this.__bValid) {
        return ARM_2D_ERR_NOT_AVAILABLE;
    }

    uint_fast8_t chSampleDataSize = 1 << this.u2SampleSize;

    //int_fast16_t iXLimit = ptROI->tSize.iWidth + ptROI->tLocation.iX; 
    int_fast16_t iYLimit = ptROI->tSize.iHeight + ptROI->tLocation.iY; 
    int_fast16_t iYStart = ptROI->tLocation.iY;

    COLOUR_INT tBrushColour = this.tBrushColour.tColour;

    int16_t iX = ptROI->tLocation.iX;
    assert(iX >= 0);
    q16_t q16PreviousSampleY;
    

    if (0 != iX) {
        arm_loader_io_seek( this.use_as__arm_generic_loader_t.tCFG.ImageIO.ptIO, 
                            this.use_as__arm_generic_loader_t.tCFG.ImageIO.pTarget, 
                            ptObj, 
                            (iX - 1) * chSampleDataSize, 
                            SEEK_SET);
    } else {
        arm_loader_io_seek( this.use_as__arm_generic_loader_t.tCFG.ImageIO.ptIO, 
                            this.use_as__arm_generic_loader_t.tCFG.ImageIO.pTarget, 
                            ptObj, 
                            iX * chSampleDataSize, 
                            SEEK_SET);
    }

    /* get previous height */
    if (!__waveform_view_get_sample_y(ptThis, &q16PreviousSampleY)) {
        return ARM_2D_ERR_NONE;
    }

    q16PreviousSampleY -= reinterpret_q16_s16((this.u5DotHeight + 1) >> 1);

    arm_loader_io_seek( this.use_as__arm_generic_loader_t.tCFG.ImageIO.ptIO, 
                        this.use_as__arm_generic_loader_t.tCFG.ImageIO.pTarget, 
                        ptObj, 
                        iX * chSampleDataSize, 
                        SEEK_SET);

    int16_t iHeight = this.tSize.iHeight;

    for (int16_t n = 0; n <= ptROI->tSize.iWidth; n++) {

        q16_t q16CurrentSampleY;

        if (!__waveform_view_get_sample_y(ptThis, &q16CurrentSampleY)) {
            return ARM_2D_ERR_NONE;
        }

        uint_fast8_t nDotHeight = this.u5DotHeight + 1; 
        q16CurrentSampleY -= reinterpret_q16_s16(nDotHeight >> 1);

        int16_t iCurrentSampleY = reinterpret_s16_q16(q16CurrentSampleY);
        int16_t iPreviousSampleY = reinterpret_s16_q16(q16PreviousSampleY);

        /* draw shadow */
        if (n < ptROI->tSize.iWidth && this.bShowShadow) {
            
            int16_t iShadowStartY = reinterpret_s16_q16(q16CurrentSampleY) + nDotHeight;
            COLOUR_INT tShadowColour = this.Shadow.tColour.tColour;

            if (this.__bShowGradient) {

                q16_t q16UpperOpacity 
                    = mul_q16(  this.Shadow.q16UpperGradientRatio,
                                reinterpret_q16_s16(iX))
                    + reinterpret_q16_s16(this.Shadow.tGradient.chTopLeft);
                                     
                q16_t q16LowerOpacity 
                    = mul_q16(  this.Shadow.q16LowerGradientRatio,
                                reinterpret_q16_s16(iX))
                    + reinterpret_q16_s16(this.Shadow.tGradient.chBottomLeft);

                q16_t q16YGradientRatio = div_n_q16((q16LowerOpacity - q16UpperOpacity),
                                                    iHeight);


                for (int16_t iY = iShadowStartY; iY < iHeight; iY++) {
                
                    if (iY >= iYLimit) {
                        break;
                    }

                    if (iY >= iYStart) {

                        uint16_t hwOpacity = reinterpret_s16_q16(
                                                q16UpperOpacity
                                            +   mul_n_q16(q16YGradientRatio, iY)
                                            +   0x8000);

                        hwOpacity = 0x100 - hwOpacity;

                        int16_t iYOffset = iY - iYStart;
                        uint8_t *pchPixel = pchBuffer + iYOffset * iTargetStrideInByte;

                        __ARM_2D_PIXEL_BLENDING(&tShadowColour, (COLOUR_INT *)pchPixel, hwOpacity);
                    }
                }

            } else {
                for (int16_t iY = iShadowStartY; iY < iHeight; iY++) {
                
                    if (iY >= iYLimit) {
                        break;
                    }

                    if (iY >= iYStart) {
                        int16_t iYOffset = iY - iYStart;
                        uint8_t *pchPixel = pchBuffer + iYOffset * iTargetStrideInByte;

                        *(COLOUR_INT *)pchPixel = tShadowColour; 
                    }
                }
            }
        }

        q16_t q16Step = q16CurrentSampleY - q16PreviousSampleY;
        if (abs_q16(q16Step) >= 0x10000) {
            int16_t iAALineStartLeft, iAALineEndLeft, iAALineStartRight, iAALineEndRight;
            bool bIncrease = false;
            if (iCurrentSampleY < iPreviousSampleY) {
                iAALineStartLeft = iCurrentSampleY;
                iAALineEndLeft = iPreviousSampleY;
                iAALineStartRight = iCurrentSampleY + nDotHeight;
                iAALineEndRight = iPreviousSampleY + nDotHeight - 1;
            } else {
                iAALineStartLeft = iPreviousSampleY + nDotHeight;
                iAALineEndLeft = iCurrentSampleY + nDotHeight;

                iAALineStartRight = iPreviousSampleY;
                iAALineEndRight = iCurrentSampleY;
                bIncrease = true;
            }
            int16_t iAALineHeight = iAALineEndLeft - iAALineStartLeft + 1;
            q16_t q16OpacityRatio = div_n_q16(reinterpret_q16_s16(256), iAALineHeight);

            if (n > 0) {
                /* draw anti-alias line */
                for (int16_t iY = iAALineStartLeft; iY < iAALineEndLeft; iY++) {
                    if (iY >= iYLimit) {
                        break;
                    }

                    if (iY >= iYStart) {
                        uint8_t chOpacity = reinterpret_s16_q16(
                                                mul_n_q16(  q16OpacityRatio, 
                                                            iY - iAALineStartLeft + 1));
                        if (!bIncrease) {
                            chOpacity = 255 - chOpacity;
                        }

                        int16_t iYOffset = iY - iYStart;
                        uint8_t *pchPixel = pchBuffer - sizeof(COLOUR_INT) + iYOffset * iTargetStrideInByte;
                        //tBrushColour = GLCD_COLOR_GREEN;
                        __ARM_2D_PIXEL_BLENDING(&tBrushColour, (COLOUR_INT *)pchPixel, chOpacity);
                    }
                }
            }

            if (n < ptROI->tSize.iWidth) {
                /* draw anti-alias line */
                for (int16_t iY = iAALineStartRight; iY < iAALineEndRight; iY++) {
                    if (iY >= iYLimit) {
                        break;
                    }

                    if (iY >= iYStart) {
                        uint8_t chOpacity = reinterpret_s16_q16(
                                                mul_n_q16(  q16OpacityRatio, 
                                                            iY - iAALineStartRight + 1));
                        if (bIncrease) {
                            chOpacity = 255 - chOpacity;
                        }

                        int16_t iYOffset = iY - iYStart;
                        uint8_t *pchPixel = pchBuffer + iYOffset * iTargetStrideInByte;

                        __ARM_2D_PIXEL_BLENDING(&tBrushColour, (COLOUR_INT *)pchPixel, chOpacity);
                    }
                }
            }

            if (n < ptROI->tSize.iWidth) {
                /* draw dot */
                for (int16_t iY = iCurrentSampleY; iY < iHeight; iY++) {
                    if (iY >= iYLimit) {
                        break;
                    }

                    if (iY >= iYStart) {
                        int16_t iYOffset = iY - iYStart;
                        uint8_t *pchPixel = pchBuffer + iYOffset * iTargetStrideInByte;
                        //tBrushColour = GLCD_COLOR_RED;
                        *(COLOUR_INT *)pchPixel = tBrushColour;
                    }

                    nDotHeight--;
                    if (0 == nDotHeight) {
                        break;
                    }
                }
            }

        } else {    /* k is small */

            if (n < ptROI->tSize.iWidth) {

                int16_t iScores = nDotHeight * 256;
                bool bFirstPixel = true;
                /* draw dot */
                for (int16_t iY = iCurrentSampleY; iY < iHeight; iY++) {
                    if (iY >= iYLimit) {
                        break;
                    }

                    uint16_t hwPixelScores;
                    uint16_t hwOpacity;
                    
                    if (bFirstPixel) {
                        bFirstPixel = false;
                        hwPixelScores = 0x100 - ((q16CurrentSampleY & 0xFF00) >> 8);

                        hwOpacity = hwPixelScores;
                    } else {
                        if (iScores >= 0x100) {
                            hwPixelScores = 0x100;
                        } else {
                            hwPixelScores = iScores;
                        }

                        hwOpacity = hwPixelScores;
                    }

                    if (iY >= iYStart) {
                        int16_t iYOffset = iY - iYStart;
                        uint8_t *pchPixel = pchBuffer + iYOffset * iTargetStrideInByte;
                        //tBrushColour = GLCD_COLOR_RED;

                        if (hwOpacity >= 0xFF) {
                            *(COLOUR_INT *)pchPixel = tBrushColour;
                        } else {
                            __ARM_2D_PIXEL_BLENDING(&tBrushColour, (COLOUR_INT *)pchPixel, 0x100 - hwOpacity);
                        }      
                    }

                    iScores -= hwPixelScores;       

                    if (iScores <= 0) {
                        break;
                    }
                }
            }
        }

        q16PreviousSampleY = q16CurrentSampleY;
        iX ++;
        pchBuffer += sizeof(COLOUR_INT);
    }

    return ARM_2D_ERR_NONE;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

#endif
