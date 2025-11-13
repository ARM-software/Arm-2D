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
#define __FOLDABLE_PANEL_IMPLEMENT__

#include "./arm_2d_example_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "foldable_panel.h"
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

enum {
    FOLDABLE_PANEL_DIRTY_REGION_LEFT    = 0,
    FOLDABLE_PANEL_DIRTY_REGION_RIGHT,
    FOLDABLE_PANEL_DIRTY_REGION_TOP,
    FOLDABLE_PANEL_DIRTY_REGION_BOTTOM,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1)
void foldable_panel_init( foldable_panel_t *ptThis,
                          foldable_panel_cfg_t *ptCFG)
{
    assert(NULL != ptThis);

    memset(ptThis, 0, sizeof(foldable_panel_t));
    if (NULL != ptCFG) {
        this.tCFG = *ptCFG;
    } else {
        this.tCFG.bShowScanLines = true;
        this.tCFG.tLineColour.tColour = GLCD_COLOR_WHITE;
    }

    if (0 == this.tCFG.u12HorizontalFoldingTimeInMS) {
        this.tCFG.u12HorizontalFoldingTimeInMS = 500;
    }
    if (0 == this.tCFG.u12VerticalFoldingTimeInMS) {
        this.tCFG.u12VerticalFoldingTimeInMS = 250;
    }

    //this.tInnerPanelSize.iHeight = 0;
    //this.tInnerPanelSize.iWidth = 0;

    if (NULL == this.tCFG.ptScene) {
        this.tCFG.bUseDirtyRegions = false;
    }

}

ARM_NONNULL(1)
void foldable_panel_depose( foldable_panel_t *ptThis)
{
    assert(NULL != ptThis);

    if (NULL != this.tCFG.ptScene) {
        arm_2d_helper_dirty_region_remove_items(   
                                    &this.tCFG.ptScene->tDirtyRegionHelper,
                                    this.DirtyRegion.tDirtyRegionItems,
                                    dimof(this.DirtyRegion.tDirtyRegionItems));
    }
}

ARM_NONNULL(1)
void foldable_panel_on_load( foldable_panel_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (this.tCFG.bUseDirtyRegions && NULL != this.tCFG.ptScene) {

        arm_2d_helper_dirty_region_add_items(   
                                    &this.tCFG.ptScene->tDirtyRegionHelper,
                                    this.DirtyRegion.tDirtyRegionItems,
                                    dimof(this.DirtyRegion.tDirtyRegionItems));
        
        arm_foreach(arm_2d_helper_dirty_region_item_t, 
                    this.DirtyRegion.tDirtyRegionItems,
                    ptItem) {
            arm_2d_helper_dirty_region_item_force_to_use_minimal_enclosure( 
                                                                    ptItem, 
                                                                    true);
            arm_2d_helper_dirty_region_item_suspend_update(ptItem, true);
        }
    }
}

ARM_NONNULL(1)
void foldable_panel_on_frame_start( foldable_panel_t *ptThis)
{
    assert(NULL != ptThis);

    if (this.tCFG.bUseDirtyRegions) {

        /* X direction */
        bool bSuspend = (   this.tInnerPanelSize.iWidth 
                        ==  this.DirtyRegion.tLastInnerPanelSize.iWidth);

        arm_2d_helper_dirty_region_item_suspend_update(
            &this.DirtyRegion.tDirtyRegionItems[FOLDABLE_PANEL_DIRTY_REGION_LEFT], 
            bSuspend);
        arm_2d_helper_dirty_region_item_suspend_update(
            &this.DirtyRegion.tDirtyRegionItems[FOLDABLE_PANEL_DIRTY_REGION_RIGHT], 
            bSuspend);

        /* Y direction */
        bSuspend = (   this.tInnerPanelSize.iHeight 
                   ==  this.DirtyRegion.tLastInnerPanelSize.iHeight);

        arm_2d_helper_dirty_region_item_suspend_update(
            &this.DirtyRegion.tDirtyRegionItems[FOLDABLE_PANEL_DIRTY_REGION_TOP], 
            bSuspend);
        arm_2d_helper_dirty_region_item_suspend_update(
            &this.DirtyRegion.tDirtyRegionItems[FOLDABLE_PANEL_DIRTY_REGION_BOTTOM], 
            bSuspend);
    }
}

static arm_fsm_rt_t __folding(foldable_panel_t *ptThis)
{
    int32_t iResult;

ARM_PT_BEGIN(this.chPT)

    /* reset time stamp */
    this.lTimestamp[0] = 0;
ARM_PT_ENTRY()
    
    if (!arm_2d_helper_time_half_cos_slider( 
            this.tOuterPanelSize.iHeight,
            0, 
            this.tCFG.u12VerticalFoldingTimeInMS,
            &iResult,
            &this.lTimestamp[0])) {
        this.tInnerPanelSize.iHeight = iResult;
        this.bInnerPanelSizeChanged = true;
        ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going);
    }

    this.tInnerPanelSize.iHeight = 0;
    this.bInnerPanelSizeChanged = true;
    
    if (!this.tCFG.bShowScanLines) {
        ARM_PT_RETURN(arm_fsm_rt_cpl);
    }

    /* reset time stamp */
    this.lTimestamp[0] = 0;
ARM_PT_ENTRY()
    int32_t iResult;
    if (!arm_2d_helper_time_half_cos_slider( 
            this.tOuterPanelSize.iWidth,
            0, 
            this.tCFG.u12HorizontalFoldingTimeInMS,
            &iResult,
            &this.lTimestamp[0])) {
        this.tInnerPanelSize.iWidth = iResult;
        this.bInnerPanelSizeChanged = true;
        ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going);
    }

    this.tInnerPanelSize.iWidth = 0;
    this.bInnerPanelSizeChanged = true;
ARM_PT_END()

    return arm_fsm_rt_cpl;
}

static arm_fsm_rt_t __unfolding(foldable_panel_t *ptThis)
{
    int32_t iResult;
ARM_PT_BEGIN(this.chPT)

    if (this.tCFG.bShowScanLines) {
        /* reset time stamp */
        this.lTimestamp[0] = 0;

ARM_PT_ENTRY()
        if (!arm_2d_helper_time_half_cos_slider( 
                0, 
                this.tOuterPanelSize.iWidth,
                this.tCFG.u12HorizontalFoldingTimeInMS,
                &iResult,
                &this.lTimestamp[0])) {
            this.tInnerPanelSize.iWidth = iResult;
            this.bInnerPanelSizeChanged = true;
            ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going);
        }
    }

    this.tInnerPanelSize.iWidth = this.tOuterPanelSize.iWidth;
    this.bInnerPanelSizeChanged = true;

    /* reset time stamp */
    this.lTimestamp[0] = 0;
ARM_PT_ENTRY()
    int32_t iResult;
    if (!arm_2d_helper_time_half_cos_slider( 
            0,
            this.tOuterPanelSize.iHeight,
            this.tCFG.u12VerticalFoldingTimeInMS,
            &iResult,
            &this.lTimestamp[0])) {
        this.tInnerPanelSize.iHeight = iResult;
        this.bInnerPanelSizeChanged = true;
        ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going);
    }

    this.tInnerPanelSize.iHeight = this.tOuterPanelSize.iHeight;
    this.bInnerPanelSizeChanged = true;

ARM_PT_END()

    return arm_fsm_rt_cpl;
}


static arm_fsm_rt_t __folding_timeline_aligned(foldable_panel_t *ptThis)
{
    int32_t iResult;
ARM_PT_BEGIN(this.chPT)

    /* reset time stamp */
    this.lTimestamp[0] = 0;
    this.lTimestamp[1] = 0;

    if (this.tCFG.u12HorizontalFoldingTimeInMS 
    >=  this.tCFG.u12VerticalFoldingTimeInMS) {

ARM_PT_ENTRY()
        uint16_t hwDelta = this.tCFG.u12HorizontalFoldingTimeInMS
                         - this.tCFG.u12VerticalFoldingTimeInMS;

        bool bComplete = arm_2d_helper_time_half_cos_slider( 
                            this.tOuterPanelSize.iWidth,
                            0,
                            this.tCFG.u12HorizontalFoldingTimeInMS,
                            &iResult,
                            &this.lTimestamp[0]);
        
        this.tInnerPanelSize.iWidth = iResult;
        this.bInnerPanelSizeChanged = true;
        
        int64_t lElapsed = arm_2d_helper_get_system_timestamp()
                         - this.lTimestamp[0];
        if (arm_2d_helper_convert_ticks_to_ms(lElapsed) >= hwDelta) {

            arm_2d_helper_time_half_cos_slider( 
                            this.tOuterPanelSize.iHeight,
                            0, 
                            this.tCFG.u12VerticalFoldingTimeInMS,
                            &iResult,
                            &this.lTimestamp[1]);

            this.tInnerPanelSize.iHeight = iResult;
        }

        if (!bComplete) {
            ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going);
        }
        this.tInnerPanelSize = (arm_2d_size_t){0,0};
        this.bInnerPanelSizeChanged = true;

    } else {

ARM_PT_ENTRY()
        uint16_t hwDelta = this.tCFG.u12VerticalFoldingTimeInMS
                         - this.tCFG.u12HorizontalFoldingTimeInMS;

        bool bComplete = arm_2d_helper_time_half_cos_slider( 
                            this.tOuterPanelSize.iHeight,
                            0, 
                            this.tCFG.u12VerticalFoldingTimeInMS,
                            &iResult,
                            &this.lTimestamp[0]);
        
        this.tInnerPanelSize.iHeight = iResult;
        this.bInnerPanelSizeChanged = true;
        
        int64_t lElapsed = arm_2d_helper_get_system_timestamp()
                         - this.lTimestamp[0];
        if (arm_2d_helper_convert_ticks_to_ms(lElapsed) >= hwDelta) {

            arm_2d_helper_time_half_cos_slider( 
                            this.tOuterPanelSize.iWidth,
                            0, 
                            this.tCFG.u12HorizontalFoldingTimeInMS,
                            &iResult,
                            &this.lTimestamp[1]);

            this.tInnerPanelSize.iWidth = iResult;
        }

        if (!bComplete) {
            ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going);
        }
        this.tInnerPanelSize = (arm_2d_size_t){0,0};
        this.bInnerPanelSizeChanged = true;
    }

ARM_PT_END()

    return arm_fsm_rt_cpl;
}

static arm_fsm_rt_t __unfolding_timeline_aligned(foldable_panel_t *ptThis)
{
    int32_t iResult;
ARM_PT_BEGIN(this.chPT)

    /* reset time stamp */
    this.lTimestamp[0] = 0;
    this.lTimestamp[1] = 0;

    if (this.tCFG.u12HorizontalFoldingTimeInMS 
    >=  this.tCFG.u12VerticalFoldingTimeInMS) {

ARM_PT_ENTRY()
        uint16_t hwDelta = this.tCFG.u12HorizontalFoldingTimeInMS
                         - this.tCFG.u12VerticalFoldingTimeInMS;

        bool bComplete = arm_2d_helper_time_half_cos_slider( 
                            0, 
                            this.tOuterPanelSize.iWidth,
                            this.tCFG.u12HorizontalFoldingTimeInMS,
                            &iResult,
                            &this.lTimestamp[0]);
        
        this.tInnerPanelSize.iWidth = iResult;
        this.bInnerPanelSizeChanged = true;
        
        int64_t lElapsed = arm_2d_helper_get_system_timestamp()
                         - this.lTimestamp[0];
        if (arm_2d_helper_convert_ticks_to_ms(lElapsed) >= hwDelta) {

            arm_2d_helper_time_half_cos_slider( 
                            0, 
                            this.tOuterPanelSize.iHeight,
                            this.tCFG.u12VerticalFoldingTimeInMS,
                            &iResult,
                            &this.lTimestamp[1]);

            this.tInnerPanelSize.iHeight = iResult;
        }

        if (!bComplete) {
            ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going);
        }
        this.tInnerPanelSize = this.tOuterPanelSize;
        this.bInnerPanelSizeChanged = true;

    } else {

ARM_PT_ENTRY()
        uint16_t hwDelta = this.tCFG.u12VerticalFoldingTimeInMS
                         - this.tCFG.u12HorizontalFoldingTimeInMS;

        bool bComplete = arm_2d_helper_time_half_cos_slider( 
                            0, 
                            this.tOuterPanelSize.iHeight,
                            this.tCFG.u12VerticalFoldingTimeInMS,
                            &iResult,
                            &this.lTimestamp[0]);
        
        this.tInnerPanelSize.iHeight = iResult;
        this.bInnerPanelSizeChanged = true;
        
        int64_t lElapsed = arm_2d_helper_get_system_timestamp()
                         - this.lTimestamp[0];
        if (arm_2d_helper_convert_ticks_to_ms(lElapsed) >= hwDelta) {

            arm_2d_helper_time_half_cos_slider( 
                            0, 
                            this.tOuterPanelSize.iWidth,
                            this.tCFG.u12HorizontalFoldingTimeInMS,
                            &iResult,
                            &this.lTimestamp[1]);

            this.tInnerPanelSize.iWidth = iResult;
        }

        if (!bComplete) {
            ARM_PT_GOTO_PREV_ENTRY(arm_fsm_rt_on_going);
        }
        this.tInnerPanelSize = this.tOuterPanelSize;
        this.bInnerPanelSizeChanged = true;
    }

ARM_PT_END()

    return arm_fsm_rt_cpl;
}

ARM_NONNULL(1)
void foldable_panel_on_frame_complete( foldable_panel_t *ptThis)
{
    assert(NULL != ptThis);
    arm_fsm_rt_t tResult = arm_fsm_rt_err;

    if (this.tCFG.bUseDirtyRegions) {
        this.DirtyRegion.tLastInnerPanelSize = this.tInnerPanelSize;
    }

    switch(this.u8Status) {
        default:
        case FOLDABLE_PANEL_STATUS_FOLDED:
            (void)ptThis;   /* make some compiler happy */
            bool bUnfoldRequest = false;

            arm_irq_safe {
                if (this.bUnfoldRequest) {
                    this.bUnfoldRequest = false;
                    bUnfoldRequest = true;
                }
            }
            if (bUnfoldRequest) {
                this.u8Status = FOLDABLE_PANEL_STATUS_UNFOLDING;
                this.chPT = 0;  /* reset PT */
            } else {
                break;
            }
            /* fall-through */
        case FOLDABLE_PANEL_STATUS_UNFOLDING:
            if (this.tCFG.bAlignTimeline) {
                tResult = __unfolding_timeline_aligned(ptThis);
            } else {
                tResult = __unfolding(ptThis);
            }
            if (arm_fsm_rt_cpl == tResult) {
                this.u8Status = FOLDABLE_PANEL_STATUS_UNFOLDED;
                this.chPT = 0;  /* reset PT */
            } else {
                break;
            }
            /* fall-through */
        case FOLDABLE_PANEL_STATUS_UNFOLDED:
            if (this.bOutPanelSizeChanged) {
                this.tInnerPanelSize = this.tOuterPanelSize;
            }

            bool bFoldRequest = false;
            arm_irq_safe {
                if (this.bFoldRequest) {
                    this.bFoldRequest = false;
                    bFoldRequest = true;
                }
            }
            if (bFoldRequest) {
                this.u8Status = FOLDABLE_PANEL_STATUS_FOLDING;
                this.chPT = 0;  /* reset PT */
            } else {
                break;
            }
            /* fall-through */
        case FOLDABLE_PANEL_STATUS_FOLDING:
            if (this.tCFG.bAlignTimeline) {
                tResult = __folding_timeline_aligned(ptThis);
            } else {
                tResult = __folding(ptThis);
            }
            if (arm_fsm_rt_cpl == tResult) {
                this.u8Status = FOLDABLE_PANEL_STATUS_FOLDED;
                this.chPT = 0;  /* reset PT */
            } else {
                break;
            }
            break;
    }

}

ARM_NONNULL(1)
foldable_panel_status_t foldable_panel_status(foldable_panel_t *ptThis)
{
    assert(NULL != ptThis);
    return this.u8Status;
}

ARM_NONNULL(1)
void foldable_panel_fold(foldable_panel_t *ptThis)
{
    assert(NULL != ptThis);

    arm_irq_safe {
        this.bFoldRequest = true;
    }
}

ARM_NONNULL(1)
void foldable_panel_unfold(foldable_panel_t *ptThis)
{
    assert(NULL != ptThis);

    arm_irq_safe {
        this.bUnfoldRequest = true;
    }
}

ARM_NONNULL(1)
arm_2d_region_t * foldable_panel_get_draw_region(foldable_panel_t *ptThis)
{
    assert(NULL != ptThis);

    return &this.tDrawRegion;
}

ARM_NONNULL(1, 2, 3)
static
void __foldable_panel_update_dirty_regions( foldable_panel_t *ptThis,
                                            const arm_2d_tile_t *ptTile, 
                                            const arm_2d_region_t *ptRegion, 
                                            bool bIsNewFrame)
{
    assert(NULL!= ptThis);
    assert(NULL!= ptTile);
    assert(NULL!= ptRegion);


    if (bIsNewFrame && this.tCFG.bUseDirtyRegions) {
        arm_2d_canvas(ptTile, __canvas) {


            arm_2d_dock_left_open((*ptRegion), 1) {

                __arm_2d_helper_dirty_region_item_update(
                    &this.DirtyRegion.tDirtyRegionItems[FOLDABLE_PANEL_DIRTY_REGION_LEFT],
                    ptTile,
                    NULL,
                    &__left_region);
            }

            arm_2d_dock_right_open((*ptRegion), 1) {

                __arm_2d_helper_dirty_region_item_update(
                    &this.DirtyRegion.tDirtyRegionItems[FOLDABLE_PANEL_DIRTY_REGION_RIGHT],
                    ptTile,
                    NULL,
                    &__right_region);
            }

            arm_2d_dock_top_open((*ptRegion), 1) {
                __arm_2d_helper_dirty_region_item_update(
                    &this.DirtyRegion.tDirtyRegionItems[FOLDABLE_PANEL_DIRTY_REGION_TOP],
                    ptTile,
                    NULL,
                    &__top_region);
            }

            arm_2d_dock_bottom_open((*ptRegion), 1) {

                __arm_2d_helper_dirty_region_item_update(
                    &this.DirtyRegion.tDirtyRegionItems[FOLDABLE_PANEL_DIRTY_REGION_BOTTOM],
                    ptTile,
                    NULL,
                    &__bottom_region);
            }
        }
    }

}


ARM_NONNULL(1)
arm_2d_tile_t * foldable_panel_show(foldable_panel_t *ptThis,
                                    const arm_2d_tile_t *ptTile, 
                                    const arm_2d_region_t *ptRegion, 
                                    bool bIsNewFrame)
{
    assert(NULL!= ptThis);
    if (-1 == (intptr_t)ptTile) {
        ptTile = arm_2d_get_default_frame_buffer();
    }

    if (bIsNewFrame) {
        arm_2d_safe_canvas_open(ptTile, __outer_panel_canvas, ptRegion) {
        
            bool bSizeChanged = false;

            if ((this.tOuterPanelSize.iWidth != __outer_panel_canvas.tSize.iWidth)
            ||  (this.tOuterPanelSize.iHeight != __outer_panel_canvas.tSize.iHeight)) {
                this.tOuterPanelSize = __outer_panel_canvas.tSize;
                bSizeChanged = true;
            }

            this.bOutPanelSizeChanged = bSizeChanged;

            if (bSizeChanged || this.bInnerPanelSizeChanged) {

                this.bInnerPanelSizeChanged = false;
                arm_2d_align_centre_open(__outer_panel_canvas, this.tInnerPanelSize) {

                    this.tDrawRegion = __centre_region;

                    arm_2d_container(ptTile, __inner_panel, &__centre_region) {
                        /* save inner panel */
                        this.tInnerPanel = __inner_panel;

                        arm_2d_region_t tUserRegion = {
                            .tLocation = {
                                .iX = __outer_panel_canvas.tLocation.iX - __centre_region.tLocation.iX,
                                .iY = __outer_panel_canvas.tLocation.iY - __centre_region.tLocation.iY,
                            },
                            .tSize = this.tOuterPanelSize,
                        };

                        arm_2d_container(&this.tInnerPanel, __user_panel, &tUserRegion) {
                            /* save user panel */
                            this.tUserPanel = __user_panel;
                        }
                    }

                    if (__centre_region.tSize.iHeight == 0 || __centre_region.tSize.iWidth == 0) {
                        /* special case */
                        this.tUserPanel.tRegion.tSize = this.tInnerPanelSize;
                    }


                    /* update dirty regions */
                    if (this.tCFG.bUseDirtyRegions && !this.tCFG.bShowScanLines) {

                        __foldable_panel_update_dirty_regions(  ptThis,
                                                                ptTile,
                                                                &__centre_region,
                                                                bIsNewFrame);

                    }
                }
            }
        }
    }

    if (this.tCFG.bShowScanLines) {
        arm_2d_container(ptTile, __outer_panel, ptRegion) {
            arm_2d_size_t tBoarderSize = this.tInnerPanelSize;

            if (this.tInnerPanelSize.iHeight != 0 || this.tInnerPanelSize.iWidth != 0) {

                if (0 == this.tInnerPanelSize.iHeight) {
                    tBoarderSize.iWidth += 2;
                    tBoarderSize.iHeight = 1;

                    arm_2d_align_centre(__outer_panel_canvas, tBoarderSize) {
                        arm_2d_fill_colour( &__outer_panel, 
                                            &__centre_region,
                                            this.tCFG.tLineColour.tColour);
                    
                        __foldable_panel_update_dirty_regions(  ptThis,
                                                                &__outer_panel,
                                                                &__centre_region,
                                                                bIsNewFrame);
                    }
                } else if (0 == this.tInnerPanelSize.iWidth) {
                    tBoarderSize.iWidth = 1;
                    tBoarderSize.iHeight += 2;

                    arm_2d_align_centre(__outer_panel_canvas, tBoarderSize) {
                        arm_2d_fill_colour( &__outer_panel, 
                                            &__centre_region,
                                            this.tCFG.tLineColour.tColour);

                        __foldable_panel_update_dirty_regions(  ptThis,
                                                                &__outer_panel,
                                                                &__centre_region,
                                                                bIsNewFrame);
                    }
                } else {
                    tBoarderSize.iWidth += 2;
                    tBoarderSize.iHeight += 2;

                    arm_2d_align_centre(__outer_panel_canvas, tBoarderSize) {
                        arm_2d_helper_draw_box( &__outer_panel, 
                                                &__centre_region,
                                                1, 
                                                this.tCFG.tLineColour.tColour,
                                                255);
                        __foldable_panel_update_dirty_regions(  ptThis,
                                                                &__outer_panel,
                                                                &__centre_region,
                                                                bIsNewFrame);
                    }
                }
            }
        }
    }


    return &this.tUserPanel;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
