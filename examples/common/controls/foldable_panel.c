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
        this.tCFG.u12HorizontalFoldingTimeInMS = 1000;
    }
    if (0 == this.tCFG.u12VerticalFoldingTimeInMS) {
        this.tCFG.u12VerticalFoldingTimeInMS = 250;
    }

    //this.tInnerPanelSize.iHeight = 0;
    //this.tInnerPanelSize.iWidth = 0;
}

ARM_NONNULL(1)
void foldable_panel_depose( foldable_panel_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void foldable_panel_on_load( foldable_panel_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void foldable_panel_on_frame_start( foldable_panel_t *ptThis)
{
    assert(NULL != ptThis);
    
}

static arm_fsm_rt_t __folding(foldable_panel_t *ptThis)
{
    int32_t iResult;

ARM_PT_BEGIN(this.chPT)

    /* reset time stamp */
    this.lTimestamp[0] = 0;
ARM_PT_ENTRY();
    
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
    
    if (!this.tCFG.bShowScanLines) {

        ARM_PT_RETURN(arm_fsm_rt_cpl);
    }

    /* reset time stamp */
    this.lTimestamp[0] = 0;
ARM_PT_ENTRY();
    int32_t iResult;
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

    this.tInnerPanelSize.iWidth = this.tOuterPanelSize.iWidth;
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

ARM_PT_ENTRY();
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
ARM_PT_ENTRY();
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

ARM_NONNULL(1)
void foldable_panel_on_frame_complete( foldable_panel_t *ptThis)
{
    assert(NULL != ptThis);

    switch(this.u8Status) {
        default:
        case FOLDABLE_PANEL_STATUS_FOLDED:
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
            if (arm_fsm_rt_cpl == __unfolding(ptThis)) {
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
        if (arm_fsm_rt_cpl == __folding(ptThis)) {
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
                }
            }
        }
    }

    if (this.tCFG.bShowScanLines) {
        arm_2d_container(ptTile, __outer_panel, ptRegion) {
            arm_2d_size_t tBoarderSize = this.tInnerPanelSize;
            tBoarderSize.iWidth += 2;
            tBoarderSize.iHeight += 2;

            arm_2d_align_centre(__outer_panel_canvas, tBoarderSize) {
                arm_2d_helper_draw_box( &__outer_panel, 
                                        &__centre_region,
                                        1, 
                                        this.tCFG.tLineColour.tColour,
                                        255);
            }
        }
    }


    return &this.tUserPanel;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
