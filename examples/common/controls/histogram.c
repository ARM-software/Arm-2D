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
#define __HISTOGRAM_IMPLEMENT__

#include "./arm_extra_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "histogram.h"
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
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wimplicit-fallthrough"
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

#define INT16_2_Q16(__VALUE)    ((int32_t)(__VALUE) << 16)
#define INT32_2_Q16(__VALUE)    ((int64_t)(__VALUE) << 16)

/*============================ TYPES =========================================*/
enum {
    HISTOGRAM_DR_START = 0,
    HISTOGRAM_DR_UPDATE_BINS,
    HISTOGRAM_DR_DONE,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

const uint8_t c_chScanLineVerticalLineMask[]= {
    255, 255, 128, 0, 128,
};

const arm_2d_tile_t c_tileLineVerticalLineMask = {
    .tRegion = {
        .tSize = {
            .iWidth = 1,
            .iHeight = 5,
        },
    },
    .tInfo = {
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {
            .chScheme = ARM_2D_COLOUR_8BIT,
        },
    },
    .pchBuffer = (uint8_t *)c_chScanLineVerticalLineMask,
};


/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
void histogram_init( histogram_t *ptThis,
                     histogram_cfg_t *ptCFG)
{
    assert(NULL != ptThis);
    assert(NULL != ptCFG);
    assert(NULL != ptCFG->Bin.ptItems);
    assert(ptCFG->Bin.hwCount > 0);
    assert(NULL != ptCFG->evtOnGetBinValue.fnHandler);

    memset(ptThis, 0, sizeof(histogram_t));

    this.tCFG = *ptCFG;


    if (this.tCFG.Bin.tSize.iWidth <= 0) {
        this.tCFG.Bin.tSize.iWidth = 1;
    }
    if (this.tCFG.Bin.tSize.iHeight <=0) {
        this.tCFG.Bin.tSize.iHeight = 64;   /* default value */
    }

    if (this.tCFG.Bin.nMaxValue <= 0) {
        this.tCFG.Bin.nMaxValue = INT32_MAX;
    }

    int16_t iBinWidth = this.tCFG.Bin.tSize.iWidth + this.tCFG.Bin.chPadding;
    if (iBinWidth < 16) {
        this.u5BinsPerDirtyRegion = ((16 + (iBinWidth - 1)) / iBinWidth) - 1;
    } else {
        this.u5BinsPerDirtyRegion = 0;
    }

    if (this.tCFG.Bin.bSupportNegative) {
        this.q16Ratio = (   INT16_2_Q16((this.tCFG.Bin.tSize.iHeight >> 1))  
                        /   this.tCFG.Bin.nMaxValue);
    } else {
        this.q16Ratio = (   INT16_2_Q16(this.tCFG.Bin.tSize.iHeight)  
                        /   this.tCFG.Bin.nMaxValue);
    }

    if (this.tCFG.Colour.wFrom == this.tCFG.Colour.wTo) {
        /* same colour */
        arm_foreach(histogram_bin_item_t, 
                    this.tCFG.Bin.ptItems, 
                    this.tCFG.Bin.hwCount,
                    ptItem) {
            memset(ptItem, 0, sizeof(histogram_bin_item_t));

            /* initialize colour */
            ptItem->tColour = arm_2d_pixel_from_brga8888(this.tCFG.Colour.wFrom);
        }
    } else {
        uint32_t wFrom = this.tCFG.Colour.wFrom;
        uint32_t wTo = this.tCFG.Colour.wTo;

        histogram_bin_item_t *ptItem = this.tCFG.Bin.ptItems;
        for (uint_fast16_t n = 0; n < this.tCFG.Bin.hwCount; n++) {
            /* initialize colour */
            ptItem->tColour = arm_2d_pixel_from_brga8888( 
                                            __arm_2d_helper_colour_slider(
                                                wFrom, 
                                                wTo,
                                                this.tCFG.Bin.hwCount,
                                                n));
            ptItem++;
        }
    }

    /* calculate the histogram size */
    this.tHistogramSize.iHeight = this.tCFG.Bin.tSize.iHeight;
    this.tHistogramSize.iWidth =    (  this.tCFG.Bin.tSize.iWidth 
                                    +  this.tCFG.Bin.chPadding)
                               * this.tCFG.Bin.hwCount;

    if (NULL != this.tCFG.ptParent) {
        this.bUseDirtyRegion = true;

        arm_2d_scene_player_dynamic_dirty_region_init(  
                                            &this.DirtyRegion.tDirtyRegionItem,
                                            this.tCFG.ptParent);
    }

}

ARM_NONNULL(1)
void histogram_depose( histogram_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (this.bUseDirtyRegion) {
        arm_2d_scene_player_dynamic_dirty_region_depose(
                                            &this.DirtyRegion.tDirtyRegionItem,
                                            this.tCFG.ptParent);
    }
}

ARM_NONNULL(1)
void histogram_on_frame_start( histogram_t *ptThis)
{
    assert(NULL != ptThis);

    if (NULL != this.tCFG.evtOnGetBinValue.fnHandler) {
        histogram_bin_item_t *ptItem = this.tCFG.Bin.ptItems;

        for (   uint_fast16_t hwBinIndex = 0; 
                hwBinIndex < this.tCFG.Bin.hwCount; 
                hwBinIndex++) {

            ptItem->iLastValue = ptItem->iCurrentValue;
            int32_t nValue = ARM_2D_INVOKE(this.tCFG.evtOnGetBinValue.fnHandler,
                                            ARM_2D_PARAM(
                                            this.tCFG.evtOnGetBinValue.pTarget,
                                            ptThis,
                                            hwBinIndex
                                            ));
            ptItem->iCurrentValue 
                =(int16_t)( (   (int64_t)this.q16Ratio 
                            *   (int64_t)INT32_2_Q16(nValue)) 
                          >> 32);
        
            ptItem++;
        }
    }

    if (this.bUseDirtyRegion) {
        arm_2d_dynamic_dirty_region_on_frame_start(
                                            &this.DirtyRegion.tDirtyRegionItem,
                                            HISTOGRAM_DR_START);
    }
}

ARM_NONNULL(1)
void histogram_show(histogram_t *ptThis,
                    const arm_2d_tile_t *ptTile, 
                    const arm_2d_region_t *ptRegion,
                    uint8_t chOpacity)
{
    assert(NULL!= ptThis);

    arm_2d_container(ptTile, __histogram, ptRegion) {

        arm_2d_region_t tPanelRegion = {
            .tSize = this.tHistogramSize,
        };

        ARM_2D_UNUSED(__histogram_canvas);

        arm_2d_container(&__histogram, __panel, &tPanelRegion) {

            arm_2d_location_t tBaseLine = {0,0};
            int16_t iBinWidth = this.tCFG.Bin.tSize.iWidth;

            tBaseLine.iY = this.tHistogramSize.iHeight >> (!!this.tCFG.Bin.bSupportNegative);
        

            arm_foreach(histogram_bin_item_t, 
                        this.tCFG.Bin.ptItems, 
                        this.tCFG.Bin.hwCount,
                        ptItem) {
            
                int16_t iHeight = ptItem->iCurrentValue;

                arm_2d_region_t tBinRegion = {
                    .tLocation = {
                        .iX = tBaseLine.iX,
                        .iY = tBaseLine.iY - iHeight * (iHeight > 0),
                    },
                    .tSize = {
                        .iWidth = iBinWidth,
                        .iHeight = ABS(iHeight),
                    },
                };

                if (this.tCFG.Bin.bUseScanLine) {
                
                    if (iHeight > 0) {
                        arm_2d_container(&__panel, __bin, &tBinRegion) {

                            int16_t iOffset = this.tHistogramSize.iHeight - iHeight;
                            arm_2d_region_t tOriginalRegion = __bin_canvas;
                            tOriginalRegion.tLocation.iY -= this.tHistogramSize.iHeight - iHeight;
                            tOriginalRegion.tSize.iHeight += iOffset;

                            arm_2d_fill_colour_with_vertical_line_mask_and_opacity(
                                &__bin,
                                &tOriginalRegion,
                                &c_tileLineVerticalLineMask,
                                (__arm_2d_color_t) {ptItem->tColour},
                                chOpacity);
                        }
                    } else {
                        arm_2d_fill_colour_with_vertical_line_mask_and_opacity(
                                &__panel,
                                &tBinRegion,
                                &c_tileLineVerticalLineMask,
                                (__arm_2d_color_t) {ptItem->tColour},
                                chOpacity);
                    }

                } else if (this.tCFG.Bin.bDrawEndPointOnly) {
                    arm_2d_location_t tPointLocation = tBinRegion.tLocation;
                    if (iHeight < 0) {
                        tPointLocation.iY -= iHeight;
                    }

                    arm_2d_draw_point(
                        &__panel,
                        tPointLocation,
                        ptItem->tColour,
                        chOpacity
                    );
                } else {
                    arm_2d_fill_colour_with_opacity(
                        &__panel,
                        &tBinRegion,
                        (__arm_2d_color_t) {ptItem->tColour},
                        chOpacity
                    );
                }

                ARM_2D_OP_WAIT_ASYNC();

                tBaseLine.iX += iBinWidth + this.tCFG.Bin.chPadding;

                ptItem++;
            }

            /* update dirty region */
            switch (arm_2d_dynamic_dirty_region_wait_next(
                                        &this.DirtyRegion.tDirtyRegionItem)) {
                case HISTOGRAM_DR_START:
                    if (this.tCFG.Bin.hwCount > 0 && NULL != this.tCFG.Bin.ptItems) {
                        this.DirtyRegion.hwCurrentBin = 0;
                    } else {
                        arm_2d_dynamic_dirty_region_change_user_region_index_only(
                            &this.DirtyRegion.tDirtyRegionItem,
                            HISTOGRAM_DR_DONE
                        );
                        break;
                    }
                    //fallthrough
                case HISTOGRAM_DR_UPDATE_BINS: {
                    bool bValueChanged = false;
                    uint_fast8_t chBinCount = this.u5BinsPerDirtyRegion + 1;
                    arm_2d_region_t tRedrawRegion = {0};
                    bool bFirstBin = true;
                    do {
                        histogram_bin_item_t *ptItem 
                            = &this.tCFG.Bin.ptItems[this.DirtyRegion.hwCurrentBin++];

                        if (ptItem->iCurrentValue != ptItem->iLastValue) {
                            
                            int16_t iBinWidth = this.tCFG.Bin.tSize.iWidth;

                            arm_2d_location_t tBaseLine = {
                                .iX = (iBinWidth + this.tCFG.Bin.chPadding) * (this.DirtyRegion.hwCurrentBin - 1),
                                .iY = this.tHistogramSize.iHeight >> (!!this.tCFG.Bin.bSupportNegative),
                            };

                            arm_2d_region_t tBinRegion = {
                                .tLocation = {
                                    .iX = tBaseLine.iX,
                                    .iY = tBaseLine.iY - ptItem->iCurrentValue,
                                },
                                .tSize = {
                                    .iWidth = iBinWidth,
                                    .iHeight = 1,
                                },
                            };

                            arm_2d_region_t tLastBinRegion = {
                                .tLocation = {
                                    .iX = tBaseLine.iX,
                                    .iY = tBaseLine.iY - ptItem->iLastValue,
                                },
                                .tSize = {
                                    .iWidth = iBinWidth,
                                    .iHeight = 1,
                                },
                            };
                            
                            arm_2d_region_t tBinRedrawRegion;
                            arm_2d_region_get_minimal_enclosure(&tLastBinRegion,
                                                                &tBinRegion,
                                                                &tBinRedrawRegion);
                            
                            if (arm_2d_region_intersect(&tBinRedrawRegion, 
                                                        &__panel_canvas, 
                                                        &tBinRedrawRegion)) {
                                bValueChanged = true;
                                if (bFirstBin) {
                                    bFirstBin = false;
                                    tRedrawRegion = tBinRedrawRegion;
                                } else {
                                    arm_2d_region_get_minimal_enclosure(&tBinRedrawRegion,
                                                                        &tRedrawRegion,
                                                                        &tRedrawRegion);
                                }
                            }
                        }

                        bool bNoMoreBin = this.DirtyRegion.hwCurrentBin >= this.tCFG.Bin.hwCount;

                        if (--chBinCount && !bNoMoreBin) {
                            continue;
                        } else if (bNoMoreBin) {

                            if (bValueChanged) {
                                arm_2d_dynamic_dirty_region_update(
                                    &this.DirtyRegion.tDirtyRegionItem,
                                    &__panel,
                                    &tRedrawRegion,
                                    HISTOGRAM_DR_DONE);
                            } else {
                                /* encounter the final bin and nothing changed */
                                arm_2d_dynamic_dirty_region_change_user_region_index_only(
                                    &this.DirtyRegion.tDirtyRegionItem,
                                    HISTOGRAM_DR_DONE);
                            }

                            break;
                        } else if (!bValueChanged) {
                            chBinCount = this.u5BinsPerDirtyRegion + 1;
                            continue;
                        }

                        arm_2d_dynamic_dirty_region_update(
                                &this.DirtyRegion.tDirtyRegionItem,
                                &__panel,
                                &tRedrawRegion,
                                HISTOGRAM_DR_UPDATE_BINS);

                        break;
                        
                    } while(true);

                    break;
                }    
                case HISTOGRAM_DR_DONE:
                    break;
                default:
                    break;
            }

            
        }
    }

    ARM_2D_OP_WAIT_ASYNC();
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
