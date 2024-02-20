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

/*============================ INCLUDES ======================================*/
#include "./shape_round_corner_box.h"
#include "./__common.h"
#include "arm_2d.h"
#include "../../../Helper/Include/arm_2d_helper.h"
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



extern const arm_2d_tile_t c_tileCircleMask;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

const arm_2d_tile_t c_tileCircleAlphaQ4 = 
    impl_child_tile(c_tileCircleMask, 7, 7, 7, 7);

const arm_2d_tile_t c_tileCircleAlphaQ1 = 
    impl_child_tile(c_tileCircleMask, 7, 0, 7, 7);

const arm_2d_tile_t c_tileCircleAlphaQ2 = 
    impl_child_tile(c_tileCircleMask, 0, 0, 7, 7);

const arm_2d_tile_t c_tileCircleAlphaQ3 = 
    impl_child_tile(c_tileCircleMask, 0, 7, 7, 7);


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
    assert(ptCircleMask->tInfo.tColourInfo.chScheme == ARM_2D_COLOUR_8BIT);

    int16_t iCircleWidth = ((ptCircleMask->tRegion.tSize.iWidth + 1) >> 1);
    int16_t iCircleHeight = ((ptCircleMask->tRegion.tSize.iHeight + 1) >> 1);

    ARM_2D_UNUSED(bIsNewFrame);
    impl_heap_fb(tileWhiteDotMask, iCircleWidth, iCircleHeight, uint8_t) {

        tileWhiteDotMask.bHasEnforcedColour = true;
        tileWhiteDotMask.tInfo.tColourInfo.chScheme = ARM_2D_COLOUR_MASK_A8;

        arm_2d_container(ptTarget, __box, ptRegion) {

            int16_t iBoxWidth = MIN(__box_canvas.tSize.iWidth, ptSource->tRegion.tSize.iWidth);
            int16_t iBoxHeight = MIN(__box_canvas.tSize.iHeight, ptSource->tRegion.tSize.iHeight);

            do {
                arm_2d_tile_t c_tileWhiteDotAlphaQ2 = 
                    impl_child_tile(*ptCircleMask, 0, 0, iCircleWidth, iCircleHeight);
                
                arm_2d_tile_t *ptileMask = &c_tileWhiteDotAlphaQ2;

                if (chOpacity < 255) {
                    memset(tileWhiteDotMask.pchBuffer, 0, get_tile_buffer_pixel_count(tileWhiteDotMask));

                    arm_2d_gray8_fill_colour_with_mask_and_opacity(
                        &tileWhiteDotMask,
                        NULL,
                        &c_tileWhiteDotAlphaQ2,
                        (arm_2d_color_gray8_t){0xFF},
                        chOpacity
                    );

                    ptileMask = &tileWhiteDotMask;
                }

                arm_2d_tile_t c_tileSourceQ2 = 
                    impl_child_tile(*ptSource, 0, 0, iCircleWidth, iCircleHeight);

                arm_2d_align_top_left(__box_canvas, c_tileWhiteDotAlphaQ2.tRegion.tSize) {

                    arm_2d_tile_copy_with_src_mask_only(&c_tileSourceQ2,
                                                        ptileMask,
                                                        &__box,
                                                        &__top_left_region);
                                                                
                    arm_2d_op_wait_async(NULL);
                }
            } while(0);

            //! copy the top right corner
            do {
                arm_2d_tile_t c_tileWhiteDotAlphaQ1 = 
                    impl_child_tile(*ptCircleMask, iCircleWidth, 0, iCircleWidth, iCircleHeight);
                
                arm_2d_tile_t *ptileMask = &c_tileWhiteDotAlphaQ1;

                if (chOpacity < 255) {
                    memset(tileWhiteDotMask.pchBuffer, 0, get_tile_buffer_pixel_count(tileWhiteDotMask));

                    arm_2d_gray8_fill_colour_with_mask_and_opacity(
                        &tileWhiteDotMask,
                        NULL,
                        &c_tileWhiteDotAlphaQ1,
                        (arm_2d_color_gray8_t){0xFF},
                        chOpacity
                    );

                    ptileMask = &tileWhiteDotMask;
                }

                arm_2d_tile_t c_tileSourceQ1 = 
                    impl_child_tile(*ptSource, 
                                    iBoxWidth - iCircleWidth, 
                                    0, 
                                    iCircleWidth, 
                                    iCircleHeight);

                arm_2d_align_top_right(__box_canvas, c_tileWhiteDotAlphaQ1.tRegion.tSize) {
                                        
                    arm_2d_tile_copy_with_src_mask_only(&c_tileSourceQ1,
                                                        ptileMask,
                                                        &__box,
                                                        &__top_right_region);

                    arm_2d_op_wait_async(NULL);
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
                
                arm_2d_op_wait_async(NULL);
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
                
                arm_2d_op_wait_async(NULL);
            }

            //! copy the bottom left corner
            do {
                arm_2d_tile_t c_tileWhiteDotAlphaQ3 = 
                    impl_child_tile(*ptCircleMask, 0, iCircleHeight, iCircleWidth, iCircleHeight);

                arm_2d_tile_t *ptileMask = &c_tileWhiteDotAlphaQ3;

                if (chOpacity < 255) {
                    memset(tileWhiteDotMask.pchBuffer, 0, get_tile_buffer_pixel_count(tileWhiteDotMask));

                    arm_2d_gray8_fill_colour_with_mask_and_opacity(
                        &tileWhiteDotMask,
                        NULL,
                        &c_tileWhiteDotAlphaQ3,
                        (arm_2d_color_gray8_t){0xFF},
                        chOpacity
                    );

                    ptileMask = &tileWhiteDotMask;
                }

                arm_2d_tile_t c_tileSourceQ3 = 
                    impl_child_tile(*ptSource, 
                                    0, 
                                    iBoxHeight - iCircleHeight, 
                                    iCircleWidth, 
                                    iCircleHeight);

                arm_2d_align_bottom_left(__box_canvas, c_tileWhiteDotAlphaQ3.tRegion.tSize) {
                                        
                    arm_2d_tile_copy_with_src_mask_only(&c_tileSourceQ3,
                                                        ptileMask,
                                                        &__box,
                                                        &__bottom_left_region);

                    arm_2d_op_wait_async(NULL);
                }
            } while(0);

            //! copy the bottom right corner
            do {
                arm_2d_tile_t c_tileWhiteDotAlphaQ4 = 
                    impl_child_tile(*ptCircleMask, iCircleWidth, iCircleHeight, iCircleWidth, iCircleHeight);

                arm_2d_tile_t *ptileMask = &c_tileWhiteDotAlphaQ4;

                if (chOpacity < 255) {
                    memset(tileWhiteDotMask.pchBuffer, 0, get_tile_buffer_pixel_count(tileWhiteDotMask));

                    arm_2d_gray8_fill_colour_with_mask_and_opacity(
                        &tileWhiteDotMask,
                        NULL,
                        &c_tileWhiteDotAlphaQ4,
                        (arm_2d_color_gray8_t){0xFF},
                        chOpacity
                    );

                    ptileMask = &tileWhiteDotMask;
                }

                arm_2d_tile_t c_tileSourceQ4 = 
                    impl_child_tile(*ptSource, 
                                    iBoxWidth - iCircleWidth, 
                                    iBoxHeight - iCircleHeight, 
                                    iCircleWidth, 
                                    iCircleHeight);

                arm_2d_align_bottom_right(__box_canvas, c_tileWhiteDotAlphaQ4.tRegion.tSize) {
                                        
                    arm_2d_tile_copy_with_src_mask_only(&c_tileSourceQ4,
                                                        ptileMask,
                                                        &__box,
                                                        &__bottom_right_region);

                    arm_2d_op_wait_async(NULL);
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
                
                arm_2d_op_wait_async(NULL);
            }
        }
    }
}

void draw_round_corner_border(  const arm_2d_tile_t *ptTarget,
                                const arm_2d_region_t *ptRegion,
                                COLOUR_INT tColour,
                                arm_2d_border_opacity_t Opacity,
                                arm_2d_corner_opacity_t CornerOpacity)
{
    assert( NULL != ptTarget );

    arm_2d_region_t tTempRegion = {0};
    if (NULL == ptRegion) {
        tTempRegion.tSize = ptTarget->tRegion.tSize;
        ptRegion = (const arm_2d_region_t *)&tTempRegion;
    }
    
    arm_2d_region_t tDrawRegion = *ptRegion;
    
    tDrawRegion.tSize.iHeight = 2;
    tDrawRegion.tSize.iWidth -= 14;
    tDrawRegion.tLocation.iX += 7;
    /* draw the top horizontal line */
    arm_2d_fill_colour_with_opacity(ptTarget,
                                    &tDrawRegion,
                                    (__arm_2d_color_t){tColour},
                                    Opacity.chTop);
    
    arm_2d_op_wait_async(NULL);
    
    tDrawRegion.tLocation.iY += ptRegion->tSize.iHeight - 2;
    
    /* draw the bottom horizontal line */
    arm_2d_fill_colour_with_opacity(ptTarget,
                                    &tDrawRegion,
                                    (__arm_2d_color_t){tColour},
                                    Opacity.chBottom);
    
    arm_2d_op_wait_async(NULL);
    
    tDrawRegion = *ptRegion;
    
    /* draw left vertical line */
    tDrawRegion.tSize.iWidth = 2;
    tDrawRegion.tLocation.iY += 7;
    tDrawRegion.tSize.iHeight -= 14;

    arm_2d_fill_colour_with_opacity(ptTarget,
                                    &tDrawRegion,
                                    (__arm_2d_color_t){tColour},
                                    Opacity.chLeft);
    
    arm_2d_op_wait_async(NULL);
    
    /* draw right vertical line */
    tDrawRegion.tLocation.iX += ptRegion->tSize.iWidth - 2;
    arm_2d_fill_colour_with_opacity(ptTarget,
                                    &tDrawRegion,
                                    (__arm_2d_color_t){tColour},
                                    Opacity.chRight);
    
    arm_2d_op_wait_async(NULL);


    
    tDrawRegion = *ptRegion;

    //! copy the top left corner
    arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tDrawRegion, 
                                            &c_tileCircleAlphaQ2, 
                                            (__arm_2d_color_t){tColour},
                                            CornerOpacity.chTopLeft);
                                                
    arm_2d_op_wait_async(NULL);

    //! copy the top right corner
    tDrawRegion.tLocation.iX += ptRegion->tSize.iWidth - c_tileCircleAlphaQ1.tRegion.tSize.iWidth;
                            
    arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tDrawRegion, 
                                            &c_tileCircleAlphaQ1, 
                                            (__arm_2d_color_t){tColour},
                                            CornerOpacity.chTopRight);

    arm_2d_op_wait_async(NULL);

    //! copy the bottom right corner 
    tDrawRegion.tLocation.iY += ptRegion->tSize.iHeight - c_tileCircleAlphaQ4.tRegion.tSize.iHeight;

    arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tDrawRegion, 
                                            &c_tileCircleAlphaQ4, 
                                            (__arm_2d_color_t){tColour},
                                            CornerOpacity.chBottomRight);

    arm_2d_op_wait_async(NULL);

    //! copy the bottom left corner 
    tDrawRegion.tLocation.iX = ptRegion->tLocation.iX;
                            
    arm_2d_fill_colour_with_mask_and_opacity(  
                                            ptTarget, 
                                            &tDrawRegion, 
                                            &c_tileCircleAlphaQ3, 
                                            (__arm_2d_color_t){tColour},
                                            CornerOpacity.chBottomLeft);

    arm_2d_op_wait_async(NULL);

}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
