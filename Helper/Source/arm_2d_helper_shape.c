/*
 * Copyright (c) 2009-2021 Arm Limited. All rights reserved.
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

/* ----------------------------------------------------------------------
 * Project:      Arm-2D Library
 * Title:        #include "arm_2d_helper_shape.c"
 * Description:  the helper service source code for drawing simple shapes
 *
 * $Date:        20. June 2024
 * $Revision:    V.1.12.0
 *
 * Target Processor:  Cortex-M cores
 * -------------------------------------------------------------------- */

/*============================ INCLUDES ======================================*/
#include "arm_2d_helper.h"
#include <math.h>
#include <time.h>
#include <assert.h>

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
#   pragma clang diagnostic ignored "-Winitializer-overrides"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern
const arm_2d_tile_t c_tileWhiteDotAlphaQuarter;


/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

ARM_NONNULL(1)
void __draw_round_corner_box( const arm_2d_tile_t *ptTarget, 
                            const arm_2d_region_t *ptRegion,
                            COLOUR_INT tColour,
                            uint8_t chOpacity,
                            bool bIsNewFrame,
                            const arm_2d_tile_t *ptCircleMask)
{
    assert(NULL != ptTarget);
    assert(NULL != ptCircleMask);
    assert(ptCircleMask->tInfo.tColourInfo.chScheme == ARM_2D_COLOUR_8BIT);

    int16_t iCircleWidth = ((ptCircleMask->tRegion.tSize.iWidth + 1) >> 1);
    int16_t iCircleHeight = ((ptCircleMask->tRegion.tSize.iHeight + 1) >> 1);

    ARM_2D_UNUSED(bIsNewFrame);
    uint16_t hwFillAlpha = (0xFF == chOpacity) ? 0xFF : (0xFF * chOpacity) >> 8;
    
    arm_2d_container(ptTarget, __box, ptRegion) {

        if (!arm_2d_helper_pfb_is_region_being_drawing(&__box, NULL, NULL)) {
            break;
        }

        do {
            arm_2d_tile_t c_tileWhiteDotAlphaQ2 = 
                impl_child_tile(*ptCircleMask, 0, 0, iCircleWidth, iCircleHeight);

            //! copy the top left corner
            arm_2d_fill_colour_with_mask_and_opacity(   
                                                    &__box, 
                                                    &__box_canvas, 
                                                    &c_tileWhiteDotAlphaQ2, 
                                                    (__arm_2d_color_t){tColour},
                                                    chOpacity);
                                                        
            arm_2d_op_wait_async(NULL);
        } while(0);

        do {
            arm_2d_tile_t c_tileWhiteDotAlphaQ1 = 
                impl_child_tile(*ptCircleMask, iCircleWidth, 0, iCircleWidth, iCircleHeight);

            //! copy the top right corner
            arm_2d_align_top_right(__box_canvas, c_tileWhiteDotAlphaQ1.tRegion.tSize) {
                                    
                arm_2d_fill_colour_with_mask_and_opacity(   
                                                        &__box, 
                                                        &__top_right_region, 
                                                        &c_tileWhiteDotAlphaQ1, 
                                                        (__arm_2d_color_t){tColour},
                                                        chOpacity);

                arm_2d_op_wait_async(NULL);
            }
        } while(0);

        /* top bar */
        arm_2d_align_top_centre(__box_canvas,
                                __box_canvas.tSize.iWidth - iCircleWidth * 2,
                                iCircleHeight) {

            arm_2d_fill_colour_with_opacity(   
                &__box, 
                &__top_centre_region, 
                (__arm_2d_color_t){tColour},
                hwFillAlpha);
            
            arm_2d_op_wait_async(NULL);
        }

        /* center bar */
        arm_2d_align_centre(__box_canvas,
                            __box_canvas.tSize.iWidth,
                            __box_canvas.tSize.iHeight - iCircleHeight * 2) {
            
            arm_2d_fill_colour_with_opacity(   
                &__box, 
                &__centre_region, 
                (__arm_2d_color_t){tColour},
                hwFillAlpha);
            
            arm_2d_op_wait_async(NULL);
        }

        do {
            arm_2d_tile_t c_tileWhiteDotAlphaQ3 = 
                impl_child_tile(*ptCircleMask, 0, iCircleHeight, iCircleWidth, iCircleHeight);

            //! copy the bottom left corner
            arm_2d_align_bottom_left(__box_canvas, c_tileWhiteDotAlphaQ3.tRegion.tSize) {
                                    
                arm_2d_fill_colour_with_mask_and_opacity(   
                                                        &__box, 
                                                        &__bottom_left_region, 
                                                        &c_tileWhiteDotAlphaQ3, 
                                                        (__arm_2d_color_t){tColour},
                                                        chOpacity);

                arm_2d_op_wait_async(NULL);
            }
        } while(0);

        do {
            arm_2d_tile_t c_tileWhiteDotAlphaQ4 = 
                impl_child_tile(*ptCircleMask, iCircleWidth, iCircleHeight, iCircleWidth, iCircleHeight);

            //! copy the bottom right corner
            arm_2d_align_bottom_right(__box_canvas, c_tileWhiteDotAlphaQ4.tRegion.tSize) {
                                    
                arm_2d_fill_colour_with_mask_and_opacity(   
                                                        &__box, 
                                                        &__bottom_right_region, 
                                                        &c_tileWhiteDotAlphaQ4, 
                                                        (__arm_2d_color_t){tColour},
                                                        chOpacity);

                arm_2d_op_wait_async(NULL);
            }
        } while(0);

        /* bottom bar */
        arm_2d_align_bottom_centre( __box_canvas,
                                    __box_canvas.tSize.iWidth - iCircleWidth * 2,
                                    iCircleHeight) {

            arm_2d_fill_colour_with_opacity(   
                &__box, 
                &__bottom_centre_region, 
                (__arm_2d_color_t){tColour},
                hwFillAlpha);
            
            arm_2d_op_wait_async(NULL);
        }
    }
}



ARM_NONNULL(1)
void __draw_round_corner_image( const arm_2d_tile_t *ptSource,
                                const arm_2d_tile_t *ptTarget, 
                                const arm_2d_region_t *ptRegion,
                                bool bIsNewFrame,
                                uint8_t chOpacity,
                                const arm_2d_tile_t *ptCircleMask)
{
    assert(NULL != ptTarget);
    assert(NULL != ptCircleMask);

    int16_t iCircleWidth = ((ptCircleMask->tRegion.tSize.iWidth + 1) >> 1);
    int16_t iCircleHeight = ((ptCircleMask->tRegion.tSize.iHeight + 1) >> 1);

    ARM_2D_UNUSED(bIsNewFrame);

    arm_2d_container(ptTarget, __box, ptRegion) {

        if (!arm_2d_helper_pfb_is_region_being_drawing(&__box, NULL, NULL)) {
            break;
        }

        int16_t iBoxWidth = MIN(__box_canvas.tSize.iWidth, ptSource->tRegion.tSize.iWidth);
        int16_t iBoxHeight = MIN(__box_canvas.tSize.iHeight, ptSource->tRegion.tSize.iHeight);

        do {
            arm_2d_tile_t c_tileWhiteDotAlphaQ2 = 
                impl_child_tile(*ptCircleMask, 0, 0, iCircleWidth, iCircleHeight);

            arm_2d_tile_t c_tileSourceQ2 = 
                impl_child_tile(*ptSource, 0, 0, iCircleWidth, iCircleHeight);

            arm_2d_align_top_left(__box_canvas, c_tileWhiteDotAlphaQ2.tRegion.tSize) {

                arm_2d_tile_copy_with_src_mask_and_opacity_only(&c_tileSourceQ2,
                                                                &c_tileWhiteDotAlphaQ2,
                                                                &__box,
                                                                &__top_left_region,
                                                                chOpacity);  
                ARM_2D_OP_WAIT_ASYNC();
            }
        } while(0);

        //! copy the top right corner
        do {
            arm_2d_tile_t c_tileWhiteDotAlphaQ1 = 
                impl_child_tile(*ptCircleMask, iCircleWidth, 0, iCircleWidth, iCircleHeight);

            arm_2d_tile_t c_tileSourceQ1 = 
                impl_child_tile(*ptSource, 
                                iBoxWidth - iCircleWidth, 
                                0, 
                                iCircleWidth, 
                                iCircleHeight);

            arm_2d_align_top_right(__box_canvas, c_tileWhiteDotAlphaQ1.tRegion.tSize) {
                                    
                arm_2d_tile_copy_with_src_mask_and_opacity_only(&c_tileSourceQ1,
                                                                &c_tileWhiteDotAlphaQ1,
                                                                &__box,
                                                                &__top_right_region,
                                                                chOpacity);

                ARM_2D_OP_WAIT_ASYNC();
            }
            
        } while(0);

        /* top bar */
        arm_2d_align_top_centre(__box_canvas,
                                __box_canvas.tSize.iWidth - iCircleWidth * 2,
                                iCircleHeight) {

            arm_2d_tile_t c_tileSourceTopBar = 
                impl_child_tile(*ptSource, 
                                iCircleWidth, 
                                0, 
                                iBoxWidth - iCircleWidth * 2, 
                                iCircleHeight);

            arm_2d_tile_copy_with_opacity(&c_tileSourceTopBar,
                                &__box,
                                &__top_centre_region,
                                chOpacity);
            
            ARM_2D_OP_WAIT_ASYNC();
        }

        /* center bar */
        arm_2d_align_centre(__box_canvas,
                            __box_canvas.tSize.iWidth,
                            __box_canvas.tSize.iHeight - iCircleHeight * 2) {
            

            arm_2d_tile_t c_tileSourceCentreBar = 
                    impl_child_tile(*ptSource, 
                                    0,
                                    iCircleHeight, 
                                    iBoxWidth, 
                                    iBoxHeight - iCircleHeight * 2);

            arm_2d_tile_copy_with_opacity(&c_tileSourceCentreBar,
                                &__box,
                                &__centre_region,
                                chOpacity);
            
            ARM_2D_OP_WAIT_ASYNC();
        }

        //! copy the bottom left corner
        do {
            arm_2d_tile_t c_tileWhiteDotAlphaQ3 = 
                impl_child_tile(*ptCircleMask, 0, iCircleHeight, iCircleWidth, iCircleHeight);

            arm_2d_tile_t c_tileSourceQ3 = 
                impl_child_tile(*ptSource, 
                                0, 
                                iBoxHeight - iCircleHeight, 
                                iCircleWidth, 
                                iCircleHeight);

            arm_2d_align_bottom_left(__box_canvas, c_tileWhiteDotAlphaQ3.tRegion.tSize) {
                                    
                arm_2d_tile_copy_with_src_mask_and_opacity_only(&c_tileSourceQ3,
                                                                &c_tileWhiteDotAlphaQ3,
                                                                &__box,
                                                                &__bottom_left_region,
                                                                chOpacity);

                ARM_2D_OP_WAIT_ASYNC();
            }
        } while(0);

        //! copy the bottom right corner
        do {
            arm_2d_tile_t c_tileWhiteDotAlphaQ4 = 
                impl_child_tile(*ptCircleMask, iCircleWidth, iCircleHeight, iCircleWidth, iCircleHeight);

            arm_2d_tile_t c_tileSourceQ4 = 
                impl_child_tile(*ptSource, 
                                iBoxWidth - iCircleWidth, 
                                iBoxHeight - iCircleHeight, 
                                iCircleWidth, 
                                iCircleHeight);

            arm_2d_align_bottom_right(__box_canvas, c_tileWhiteDotAlphaQ4.tRegion.tSize) {
                                    
                arm_2d_tile_copy_with_src_mask_and_opacity_only(&c_tileSourceQ4,
                                                                &c_tileWhiteDotAlphaQ4,
                                                                &__box,
                                                                &__bottom_right_region,
                                                                chOpacity);

                ARM_2D_OP_WAIT_ASYNC();
            }
        } while(0);

        /* bottom bar */
        arm_2d_align_bottom_centre( __box_canvas,
                                    __box_canvas.tSize.iWidth - iCircleWidth * 2,
                                    iCircleHeight) {

            arm_2d_tile_t c_tileSourceBottomBar = 
                impl_child_tile(*ptSource, 
                                iCircleWidth,
                                iBoxHeight - iCircleHeight, 
                                iBoxWidth - iCircleWidth * 2, 
                                iCircleHeight);

            arm_2d_tile_copy_with_opacity(&c_tileSourceBottomBar,
                                &__box,
                                &__bottom_centre_region,
                                chOpacity);
            
            ARM_2D_OP_WAIT_ASYNC();
        }

    }
}

ARM_NONNULL(1)
void __draw_round_corner_border(const arm_2d_tile_t *ptTarget,
                                const arm_2d_region_t *ptRegion,
                                COLOUR_INT tColour,
                                arm_2d_border_opacity_t Opacity,
                                arm_2d_corner_opacity_t CornerOpacity,
                                const arm_2d_tile_t *ptCircleMask)
{

    arm_2d_container(ptTarget, __round_corner_box, ptRegion) {

        if (!arm_2d_helper_pfb_is_region_being_drawing(&__round_corner_box, NULL, NULL)) {
            break;
        }

        int16_t iCicleWidthHalf = ptCircleMask->tRegion.tSize.iWidth / 2;
        int16_t iCicleHeightHalf = ptCircleMask->tRegion.tSize.iWidth / 2;

        const arm_2d_tile_t tileCircleQuaterMask = 
            impl_child_tile(
                *ptCircleMask, 
                0, 
                0, 
                iCicleWidthHalf, 
                iCicleHeightHalf);

        const arm_2d_tile_t tileCircleHorizontalLineLeftMask = 
            impl_child_tile(
                *ptCircleMask, 
                0, 
                iCicleHeightHalf, 
                iCicleWidthHalf, 
                1);

        const arm_2d_tile_t tileCircleHorizontalLineRightMask = 
            impl_child_tile(
                *ptCircleMask, 
                iCicleWidthHalf, 
                iCicleHeightHalf, 
                iCicleWidthHalf, 
                1);

        const arm_2d_tile_t tileCircleVerticalLineTopMask = 
            impl_child_tile(
                *ptCircleMask, 
                iCicleWidthHalf, 
                0, 
                1, 
                iCicleHeightHalf);

        const arm_2d_tile_t tileCircleVerticalLineBottomMask = 
            impl_child_tile(
                *ptCircleMask, 
                iCicleWidthHalf, 
                iCicleHeightHalf, 
                1, 
                iCicleHeightHalf);


        /* top left corner */
        arm_2d_align_top_left(  __round_corner_box_canvas,
                                tileCircleQuaterMask.tRegion.tSize) {
            arm_2d_fill_colour_with_mask_and_opacity(   
                                                    &__round_corner_box, 
                                                    &__top_left_region, 
                                                    &tileCircleQuaterMask, 
                                                    (__arm_2d_color_t){tColour},
                                                    CornerOpacity.chTopLeft);
            ARM_2D_OP_WAIT_ASYNC();
        }

        /* top right corner */
        arm_2d_align_top_right( __round_corner_box_canvas,
                                tileCircleQuaterMask.tRegion.tSize) {
            arm_2d_fill_colour_with_mask_x_mirror_and_opacity(   
                                                    &__round_corner_box, 
                                                    &__top_right_region, 
                                                    &tileCircleQuaterMask, 
                                                    (__arm_2d_color_t){tColour},
                                                    CornerOpacity.chTopRight);
            ARM_2D_OP_WAIT_ASYNC();
        }

        /* bottom left corner */
        arm_2d_align_bottom_left(   __round_corner_box_canvas,
                                    tileCircleQuaterMask.tRegion.tSize) {
            arm_2d_fill_colour_with_mask_y_mirror_and_opacity(   
                                                    &__round_corner_box, 
                                                    &__bottom_left_region, 
                                                    &tileCircleQuaterMask, 
                                                    (__arm_2d_color_t){tColour},
                                                    CornerOpacity.chBottomLeft);
            ARM_2D_OP_WAIT_ASYNC();
        }

        /* bottom right corner */
        arm_2d_align_bottom_right(  __round_corner_box_canvas,
                                    tileCircleQuaterMask.tRegion.tSize) {
            arm_2d_fill_colour_with_mask_xy_mirror_and_opacity(   
                                                    &__round_corner_box, 
                                                    &__bottom_right_region, 
                                                    &tileCircleQuaterMask, 
                                                    (__arm_2d_color_t){tColour},
                                                    CornerOpacity.chBottomRight);
            ARM_2D_OP_WAIT_ASYNC();
        }

        arm_2d_dock_vertical(__round_corner_box_canvas,
                                __round_corner_box_canvas.tSize.iHeight
                             -  tileCircleQuaterMask.tRegion.tSize.iHeight * 2) {

            /* left border */
            arm_2d_dock_left(   __vertical_region, 
                                tileCircleHorizontalLineLeftMask.tRegion.tSize.iWidth) {

                arm_2d_fill_colour_with_horizontal_line_mask_and_opacity(
                                                    &__round_corner_box, 
                                                    &__left_region, 
                                                    &tileCircleHorizontalLineLeftMask, 
                                                    (__arm_2d_color_t){tColour},
                                                    Opacity.chLeft);
                ARM_2D_OP_WAIT_ASYNC();
            }

            /* right border */
            arm_2d_dock_right(  __vertical_region, 
                                tileCircleHorizontalLineRightMask.tRegion.tSize.iWidth) {
                
                arm_2d_fill_colour_with_horizontal_line_mask_and_opacity(
                                                    &__round_corner_box, 
                                                    &__right_region, 
                                                    &tileCircleHorizontalLineRightMask, 
                                                    (__arm_2d_color_t){tColour},
                                                    Opacity.chRight);
                ARM_2D_OP_WAIT_ASYNC();
            }
        }

        arm_2d_dock_horizontal(__round_corner_box_canvas,
                                __round_corner_box_canvas.tSize.iWidth
                             -  tileCircleQuaterMask.tRegion.tSize.iWidth * 2) {

            /* top border */
            arm_2d_dock_top(    __horizontal_region, 
                                tileCircleVerticalLineTopMask.tRegion.tSize.iHeight) {
                
                arm_2d_fill_colour_with_vertical_line_mask_and_opacity(
                                                    &__round_corner_box, 
                                                    &__top_region, 
                                                    &tileCircleVerticalLineTopMask, 
                                                    (__arm_2d_color_t){tColour},
                                                    Opacity.chTop);
                ARM_2D_OP_WAIT_ASYNC();
            }

            /* bottom border */
            arm_2d_dock_bottom( __horizontal_region, 
                                tileCircleVerticalLineBottomMask.tRegion.tSize.iHeight) {
                
                arm_2d_fill_colour_with_vertical_line_mask_and_opacity(
                                                    &__round_corner_box, 
                                                    &__bottom_region, 
                                                    &tileCircleVerticalLineBottomMask, 
                                                    (__arm_2d_color_t){tColour},
                                                    Opacity.chBottom);
                ARM_2D_OP_WAIT_ASYNC();
            }
        }
    }
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
