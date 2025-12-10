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
#define __PROGRESS_BAR_ROUND_IMPLEMENT__

#include "./arm_2d_example_controls.h"
#include "./__common.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "progress_bar_round.h"
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
extern
const arm_2d_tile_t c_tileWhiteDotMask;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1,2)
void progress_bar_round_init( progress_bar_round_t *ptThis,
                          progress_bar_round_cfg_t *ptCFG)
{
    assert(NULL!= ptThis);
    memset(ptThis, 0, sizeof(progress_bar_round_t));
    this.tCFG = *ptCFG;

    progress_bar_round_set_circle_mask(ptThis, ptCFG->ptCircleMask);

    if (0 == this.tCFG.ValueRange.iMax && 0 == this.tCFG.ValueRange.iMin) {
        this.tCFG.ValueRange.iMax = 1000;
    }
}

ARM_NONNULL(1)
void progress_bar_round_depose( progress_bar_round_t *ptThis)
{
    assert(NULL != ptThis);

    if (NULL != this.tCFG.ptScene) {
        arm_2d_helper_dirty_region_remove_items(   
            &this.tCFG.ptScene->tDirtyRegionHelper,
            &this.tDirtyRegionItem,
            1);
        
    }
}

ARM_NONNULL(1)
void progress_bar_round_on_load( progress_bar_round_t *ptThis)
{
    assert(NULL != ptThis);
    
    if (NULL != this.tCFG.ptScene) {
        arm_2d_helper_dirty_region_add_items(   
            &this.tCFG.ptScene->tDirtyRegionHelper,
            &this.tDirtyRegionItem,
            1);
        
        arm_2d_helper_dirty_region_item_force_to_use_minimal_enclosure(
                                                    &this.tDirtyRegionItem,
                                                    true);
    }
}

ARM_NONNULL(1)
void progress_bar_round_on_frame_start( progress_bar_round_t *ptThis)
{
    assert(NULL != ptThis);

    arm_2d_helper_dirty_region_item_suspend_update( &this.tDirtyRegionItem,
                                                    true);
    
}

ARM_NONNULL(1)
void progress_bar_round_on_frame_complete( progress_bar_round_t *ptThis)
{
    assert(NULL != ptThis);
    
}

ARM_NONNULL(1)
void progress_bar_round_set_circle_mask(progress_bar_round_t *ptThis, 
                                        const arm_2d_tile_t *ptCircleMask)
{
    assert(NULL != ptThis);

    this.tCFG.ptCircleMask = ptCircleMask;

    if (NULL == this.tCFG.ptCircleMask) {
        this.tCFG.ptCircleMask = &c_tileWhiteDotMask;
    }

    this.bFullyRedraw = true;

}

ARM_NONNULL(1, 2)
arm_2d_region_t progress_bar_round_show(   progress_bar_round_t *ptThis,
                                const arm_2d_tile_t *ptTarget, 
                                const arm_2d_region_t *ptRegion,
                                COLOUR_INT tBackgroundColour,
                                COLOUR_INT tBarColour,
                                int16_t iProgress,
                                uint8_t chOpacity)
{
    arm_2d_region_t tDrawRegion = {0};
    if (-1 == (intptr_t)ptTarget) {
        ptTarget = arm_2d_get_default_frame_buffer();
    }

    assert(NULL!= ptThis);
    assert(NULL != this.tCFG.ptCircleMask);

    bool bIsNewFrame = (ARM_2D_RT_TRUE == arm_2d_target_tile_is_new_frame(ptTarget));

    if (bIsNewFrame) {

        iProgress = MIN(this.tCFG.ValueRange.iMax, iProgress);
        iProgress = MAX(this.tCFG.ValueRange.iMin, iProgress);

        arm_2d_helper_dirty_region_item_suspend_update( 
                                                &this.tDirtyRegionItem,
                                                (iProgress == this.iProgress));


        this.iProgress = iProgress;

    }

    arm_2d_safe_canvas_open(ptTarget, __control_canvas, ptRegion) {

        arm_2d_size_t tBarSize = {
            .iWidth = __control_canvas.tSize.iWidth,
            .iHeight = this.tCFG.ptCircleMask->tRegion.tSize.iHeight,
        };
        if (NULL == ptRegion) {
            tBarSize.iWidth = tBarSize.iWidth * 3 >> 3; //!< 3/8 Width
        }

        arm_2d_align_centre_open(__control_canvas, tBarSize) {
            tDrawRegion = __centre_region;
            arm_2d_container(ptTarget, __progress_bar, &__centre_region) {
        
                int16_t iProgressBarLength = __progress_bar_canvas.tSize.iWidth;
                if (bIsNewFrame) {

                    int16_t iFullLength = this.tCFG.ValueRange.iMax - this.tCFG.ValueRange.iMin;
                    this.q16Ratio 
                        = div_n_q16(reinterpret_q16_s16(iProgressBarLength),
                                    iFullLength);
                }

                draw_round_corner_box(  &__progress_bar, 
                                        NULL, 
                                        tBackgroundColour, 
                                        chOpacity,
                                        true,
                                        this.tCFG.ptCircleMask);
                
                int16_t iBarLength 
                    = reinterpret_s16_q16(
                        mul_n_q16(  this.q16Ratio, 
                                    this.iProgress - this.tCFG.ValueRange.iMin));
                if (this.iProgress == this.tCFG.ValueRange.iMax) {
                    iBarLength = iProgressBarLength;
                }

                int16_t iHalfCircleWidth = this.tCFG.ptCircleMask->tRegion.tSize.iWidth >> 1;
                int16_t iStrideMaxWidth = __progress_bar_canvas.tSize.iWidth - iHalfCircleWidth;

                arm_2d_dock_left(__progress_bar_canvas, iBarLength) {
                    do {
                        arm_2d_fill_colour_with_mask_and_opacity(
                                &__progress_bar,
                                &__left_region,
                                this.tCFG.ptCircleMask,
                                (__arm_2d_color_t){tBarColour}, 
                                chOpacity);
                        if (iBarLength <= iHalfCircleWidth) {
                            break;
                        }

                        arm_2d_region_t tTempRegion = __left_region;
                        tTempRegion.tLocation.iX += iHalfCircleWidth;
                        tTempRegion.tSize.iWidth -= iHalfCircleWidth;

                        if ( iBarLength <= iStrideMaxWidth) {
                            arm_2d_fill_colour_with_opacity(&__progress_bar, 
                                &tTempRegion, 
                                (__arm_2d_color_t){tBarColour}, 
                                chOpacity );
                            break;
                        }

                        tTempRegion.tSize.iWidth = iStrideMaxWidth - iHalfCircleWidth;

                        arm_2d_fill_colour_with_opacity(&__progress_bar, 
                                                        &tTempRegion, 
                                                        (__arm_2d_color_t){tBarColour}, 
                                                        chOpacity );
                        
                        arm_2d_dock_right(__progress_bar_canvas, iHalfCircleWidth) {

                            if (arm_2d_region_intersect(&__left_region, &__right_region, &tTempRegion)) {
                                arm_2d_tile_t tRightCircileMask = 
                                    impl_child_tile(
                                        *this.tCFG.ptCircleMask, 
                                        iHalfCircleWidth, 
                                        0, 
                                        iHalfCircleWidth, 
                                        this.tCFG.ptCircleMask->tRegion.tSize.iHeight);

                                arm_2d_fill_colour_with_mask_and_opacity(
                                    &__progress_bar,
                                    &tTempRegion,
                                    &tRightCircileMask,
                                    (__arm_2d_color_t){tBarColour}, 
                                    chOpacity);

                            }
                        }
                            
                    } while(0);

                    /* tracking the right edge to update the dirty region */
                    if (!this.bFullyRedraw) {
                        arm_2d_dock_right(__left_region, 1) {
                            arm_2d_helper_dirty_region_update_item( 
                                                            &this.tDirtyRegionItem,
                                                            &__progress_bar,
                                                            &__progress_bar_canvas,
                                                            &__right_region);
                        }
                    }
                }

                if (0 == iBarLength || this.bFullyRedraw) {
                    this.bFullyRedraw = false;
                    arm_2d_helper_dirty_region_update_item( 
                        &this.tDirtyRegionItem,
                        &__progress_bar,
                        NULL,
                        &__progress_bar_canvas);
                }
            }
        }

    }

    ARM_2D_OP_WAIT_ASYNC();
    
    return tDrawRegion;
}

ARM_NONNULL(1, 2)
arm_2d_region_t progress_bar_round_show2(   progress_bar_round_t *ptThis,
                                const arm_2d_tile_t *ptTarget, 
                                const arm_2d_region_t *ptRegion,
                                COLOUR_INT tBackgroundColour,
                                COLOUR_INT tBarColour,
                                int16_t iProgress,
                                uint8_t chOpacity)
{
    arm_2d_region_t tDrawRegion = {0};

    if (-1 == (intptr_t)ptTarget) {
        ptTarget = arm_2d_get_default_frame_buffer();
    }

    assert(NULL!= ptThis);
    assert(NULL != this.tCFG.ptCircleMask);

    bool bIsNewFrame = arm_2d_target_tile_is_new_frame(ptTarget);

    if (bIsNewFrame) {

        iProgress = MIN(this.tCFG.ValueRange.iMax, iProgress);
        iProgress = MAX(this.tCFG.ValueRange.iMin, iProgress);

        arm_2d_helper_dirty_region_item_suspend_update( 
                                                &this.tDirtyRegionItem,
                                                (iProgress == this.iProgress));


        this.iProgress = iProgress;

    }

    arm_2d_safe_canvas_open(ptTarget, __control_canvas, ptRegion) {

        arm_2d_size_t tBarSize = {
            .iWidth = __control_canvas.tSize.iWidth,
            .iHeight = this.tCFG.ptCircleMask->tRegion.tSize.iHeight,
        };
        if (NULL == ptRegion) {
            tBarSize.iWidth = tBarSize.iWidth * 3 >> 3; //!< 3/8 Width
        }

        arm_2d_align_centre_open(__control_canvas, tBarSize) {
            tDrawRegion = __centre_region;
            arm_2d_container(ptTarget, __progress_bar, &__centre_region) {
        
                int16_t iProgressBarLength = __progress_bar_canvas.tSize.iWidth - this.tCFG.ptCircleMask->tRegion.tSize.iWidth;

                if (bIsNewFrame) {

                    int16_t iFullLength = this.tCFG.ValueRange.iMax - this.tCFG.ValueRange.iMin;
                    this.q16Ratio 
                        = div_n_q16(reinterpret_q16_s16(iProgressBarLength),
                                    iFullLength);
                }

                draw_round_corner_box(  &__progress_bar, 
                                        NULL, 
                                        tBackgroundColour, 
                                        chOpacity,
                                        true,
                                        this.tCFG.ptCircleMask);
                
                int16_t iHalfCircleLength = this.tCFG.ptCircleMask->tRegion.tSize.iWidth >> 1;

                arm_2d_dock_left(__progress_bar_canvas, iHalfCircleLength) {

                    arm_2d_fill_colour_with_mask_and_opacity(
                        &__progress_bar,
                        &__left_region,
                        this.tCFG.ptCircleMask,
                        (__arm_2d_color_t){tBarColour}, 
                        chOpacity);
                }

                int16_t iBarLength 
                        = reinterpret_s16_q16(
                            mul_n_q16(  this.q16Ratio, 
                                        this.iProgress - this.tCFG.ValueRange.iMin));

                if (this.iProgress >= this.tCFG.ValueRange.iMax) {
                    iBarLength = iProgressBarLength;
                }

                arm_2d_dock_vertical(__progress_bar_canvas, 
                                    this.tCFG.ptCircleMask->tRegion.tSize.iHeight, 
                                    iHalfCircleLength) {

                    /* draw the stride */
                    arm_2d_dock_left(__vertical_region, iBarLength) {

                        arm_2d_fill_colour_with_opacity(&__progress_bar, 
                            &__left_region, 
                            (__arm_2d_color_t){tBarColour}, 
                            chOpacity );
                    }
                }

                /* draw the right semicircle */
                arm_2d_dock_left(__progress_bar_canvas, iBarLength + iHalfCircleLength * 2) {

                    arm_2d_dock_right(__left_region, iHalfCircleLength) {
                        arm_2d_tile_t tRightCircileMask = 
                                        impl_child_tile(
                                            *this.tCFG.ptCircleMask, 
                                            iHalfCircleLength, 
                                            0, 
                                            iHalfCircleLength, 
                                            this.tCFG.ptCircleMask->tRegion.tSize.iHeight);
    
                        arm_2d_fill_colour_with_mask_and_opacity(
                            &__progress_bar,
                            &__right_region,
                            &tRightCircileMask,
                            (__arm_2d_color_t){tBarColour}, 
                            chOpacity);
                        
                        if (!this.bFullyRedraw) {
                            /* tracking the right semicircle to update the dirty region */
                            arm_2d_helper_dirty_region_update_item( 
                                    &this.tDirtyRegionItem,
                                    &__progress_bar,
                                    &__left_region,
                                    &__right_region);
                        }
                    }

                }
                
                if (0 == iBarLength || this.bFullyRedraw) {
                    this.bFullyRedraw = false;
                    arm_2d_helper_dirty_region_update_item( 
                        &this.tDirtyRegionItem,
                        &__progress_bar,
                        NULL,
                        &__progress_bar_canvas);
                }
            }
        }

    }

    ARM_2D_OP_WAIT_ASYNC();
    
    return tDrawRegion;
}

ARM_NONNULL(1, 2)
arm_2d_region_t progress_bar_round_show3(   progress_bar_round_t *ptThis,
                                const arm_2d_tile_t *ptTarget, 
                                const arm_2d_region_t *ptRegion,
                                COLOUR_INT tBackgroundColour,
                                COLOUR_INT tBarColour,
                                int16_t iProgress,
                                uint8_t chOpacity)
{
    arm_2d_region_t tDrawRegion = {0};

    if (-1 == (intptr_t)ptTarget) {
        ptTarget = arm_2d_get_default_frame_buffer();
    }

    assert(NULL!= ptThis);
    assert(NULL != this.tCFG.ptCircleMask);

    bool bIsNewFrame = arm_2d_target_tile_is_new_frame(ptTarget);

    if (bIsNewFrame) {

        iProgress = MIN(this.tCFG.ValueRange.iMax, iProgress);
        iProgress = MAX(this.tCFG.ValueRange.iMin, iProgress);

        arm_2d_helper_dirty_region_item_suspend_update( 
                                                &this.tDirtyRegionItem,
                                                (iProgress == this.iProgress));


        this.iProgress = iProgress;

    }

    arm_2d_safe_canvas_open(ptTarget, __control_canvas, ptRegion) {

        arm_2d_size_t tBarSize = {
            .iWidth = __control_canvas.tSize.iWidth,
            .iHeight = this.tCFG.ptCircleMask->tRegion.tSize.iHeight,
        };
        if (NULL == ptRegion) {
            tBarSize.iWidth = tBarSize.iWidth * 3 >> 3; //!< 3/8 Width
        }

        arm_2d_align_centre_open(__control_canvas, tBarSize) {
            tDrawRegion = __centre_region;
            arm_2d_container(ptTarget, __progress_bar, &__centre_region) {
        
                int16_t iProgressBarLength = __progress_bar_canvas.tSize.iWidth;

                if (bIsNewFrame) {

                    int16_t iFullLength = this.tCFG.ValueRange.iMax - this.tCFG.ValueRange.iMin;
                    this.q16Ratio 
                        = div_n_q16(reinterpret_q16_s16(iProgressBarLength),
                                    iFullLength);
                }

                /* calculate the bar length */
                int16_t iBarLength 
                        = reinterpret_s16_q16(
                            mul_n_q16(  this.q16Ratio, 
                                        this.iProgress - this.tCFG.ValueRange.iMin));

                if (this.iProgress >= this.tCFG.ValueRange.iMax) {
                    iBarLength = iProgressBarLength;
                }

                /* draw background */
                draw_round_corner_box(  &__progress_bar, 
                                        NULL, 
                                        tBackgroundColour, 
                                        chOpacity,
                                        true,
                                        this.tCFG.ptCircleMask);
                
                
                int16_t iCircleLength = this.tCFG.ptCircleMask->tRegion.tSize.iWidth;
                if (iBarLength < iCircleLength) {

                    arm_2d_region_t tLeftEnd = {
                        .tLocation = __progress_bar_canvas.tLocation,
                        .tSize = this.tCFG.ptCircleMask->tRegion.tSize,
                    };

                    arm_2d_container(&__progress_bar, __left_end, &tLeftEnd) {

                        arm_2d_region_t tDrawLeftEndRegion = {
                            .tLocation = {
                                .iX = __left_end_canvas.tLocation.iX
                                    - iCircleLength
                                    + iBarLength,
                                .iY = __left_end_canvas.tLocation.iY,
                            },
                            .tSize = __left_end_canvas.tSize,
                        };
                        arm_2d_fill_colour_with_masks_and_opacity(
                            &__left_end,
                            &tDrawLeftEndRegion,
                            this.tCFG.ptCircleMask,
                            this.tCFG.ptCircleMask,
                            (__arm_2d_color_t){tBarColour}, 
                            chOpacity
                        );

                        /* tracking the right semicircle to update the dirty region */
                        arm_2d_helper_dirty_region_update_item( 
                                &this.tDirtyRegionItem,
                                &__left_end,
                                NULL,
                                &tDrawLeftEndRegion);
                    }
                    return tDrawRegion;
                }

                int16_t iHalfCircleLength = iCircleLength >> 1;

                iBarLength -= iCircleLength;

                arm_2d_dock_left(__progress_bar_canvas, iHalfCircleLength) {

                    arm_2d_fill_colour_with_mask_and_opacity(
                        &__progress_bar,
                        &__left_region,
                        this.tCFG.ptCircleMask,
                        (__arm_2d_color_t){tBarColour}, 
                        chOpacity);
                }

                arm_2d_dock_vertical(__progress_bar_canvas, 
                                    this.tCFG.ptCircleMask->tRegion.tSize.iHeight, 
                                    iHalfCircleLength) {

                    /* draw the stride */
                    arm_2d_dock_left(__vertical_region, iBarLength) {

                        arm_2d_fill_colour_with_opacity(&__progress_bar, 
                            &__left_region, 
                            (__arm_2d_color_t){tBarColour}, 
                            chOpacity );
                    }
                }

                /* draw the right semicircle */
                arm_2d_dock_left(__progress_bar_canvas, iBarLength + iHalfCircleLength * 2) {

                    arm_2d_dock_right(__left_region, iHalfCircleLength) {
                        arm_2d_tile_t tRightCircileMask = 
                                        impl_child_tile(
                                            *this.tCFG.ptCircleMask, 
                                            iHalfCircleLength, 
                                            0, 
                                            iHalfCircleLength, 
                                            this.tCFG.ptCircleMask->tRegion.tSize.iHeight);
    
                        arm_2d_fill_colour_with_mask_and_opacity(
                            &__progress_bar,
                            &__right_region,
                            &tRightCircileMask,
                            (__arm_2d_color_t){tBarColour}, 
                            chOpacity);
                        
                        if (!this.bFullyRedraw) {
                            /* tracking the right semicircle to update the dirty region */
                            arm_2d_helper_dirty_region_update_item( 
                                    &this.tDirtyRegionItem,
                                    &__progress_bar,
                                    &__left_region,
                                    &__right_region);
                        }
                    }

                }
                
                if (0 == iBarLength || this.bFullyRedraw) {
                    this.bFullyRedraw = false;
                    arm_2d_helper_dirty_region_update_item( 
                        &this.tDirtyRegionItem,
                        &__progress_bar,
                        NULL,
                        &__progress_bar_canvas);
                }
            }
        }

    }

    ARM_2D_OP_WAIT_ASYNC();
    
    return tDrawRegion;
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
