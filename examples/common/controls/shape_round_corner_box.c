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

extern
const arm_2d_tile_t c_tileWhiteDotMask;

extern const arm_2d_tile_t c_tileCircleMask;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

const arm_2d_tile_t c_tileWhiteDotAlphaQ4 = 
    impl_child_tile(c_tileWhiteDotMask, 7, 7, 7, 7);

const arm_2d_tile_t c_tileWhiteDotAlphaQ1 = 
    impl_child_tile(c_tileWhiteDotMask, 7, 0, 7, 7);

const arm_2d_tile_t c_tileWhiteDotAlphaQ2 = 
    impl_child_tile(c_tileWhiteDotMask, 0, 0, 7, 7);

const arm_2d_tile_t c_tileWhiteDotAlphaQ3 = 
    impl_child_tile(c_tileWhiteDotMask, 0, 7, 7, 7);
    

const arm_2d_tile_t c_tileCircleAlphaQ4 = 
    impl_child_tile(c_tileCircleMask, 7, 7, 7, 7);

const arm_2d_tile_t c_tileCircleAlphaQ1 = 
    impl_child_tile(c_tileCircleMask, 7, 0, 7, 7);

const arm_2d_tile_t c_tileCircleAlphaQ2 = 
    impl_child_tile(c_tileCircleMask, 0, 0, 7, 7);

const arm_2d_tile_t c_tileCircleAlphaQ3 = 
    impl_child_tile(c_tileCircleMask, 0, 7, 7, 7);


/*============================ IMPLEMENTATION ================================*/

void draw_round_corner_box( const arm_2d_tile_t *ptTarget, 
                            const arm_2d_region_t *ptRegion,
                            COLOUR_INT tColour,
                            uint8_t chOpacity,
                            bool bIsNewFrame)
{
    assert(NULL != ptTarget);
    ARM_2D_UNUSED(bIsNewFrame);
    uint16_t hwFillAlpha = (0xFF == chOpacity) ? 0xFF : (0xFF * chOpacity) >> 8;
    
    arm_2d_region_t tTempRegion = {0};
    if (NULL == ptRegion) {
        tTempRegion.tSize = ptTarget->tRegion.tSize;
        ptRegion = (const arm_2d_region_t *)&tTempRegion;
    }

    arm_2d_region_t tRegion = *ptRegion;


    //! copy the top left corner
    arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tRegion, 
                                            &c_tileWhiteDotAlphaQ2, 
                                            (__arm_2d_color_t){tColour},
                                            chOpacity);
                                                
    arm_2d_op_wait_async(NULL);

    //! copy the top right corner
    tRegion.tLocation.iX += ptRegion->tSize.iWidth - c_tileWhiteDotAlphaQ1.tRegion.tSize.iWidth;
                            
    arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tRegion, 
                                            &c_tileWhiteDotAlphaQ1, 
                                            (__arm_2d_color_t){tColour},
                                            chOpacity);

    arm_2d_op_wait_async(NULL);

    arm_2dp_fill_colour_with_opacity(   
        NULL,
        ptTarget, 
        &(arm_2d_region_t) {
            .tSize = {
                .iHeight = c_tileWhiteDotAlphaQ4.tRegion.tSize.iHeight,
                .iWidth = tRegion.tSize.iWidth - c_tileWhiteDotAlphaQ4.tRegion.tSize.iWidth * 2,
            },
            .tLocation = {
                .iX = ptRegion->tLocation.iX + c_tileWhiteDotAlphaQ4.tRegion.tSize.iWidth,
                .iY = ptRegion->tLocation.iY,
            },
        }, 
        (__arm_2d_color_t){tColour},
        hwFillAlpha);
    
    arm_2d_op_wait_async(NULL);

    arm_2dp_fill_colour_with_opacity(   
        NULL,
        ptTarget, 
        &(arm_2d_region_t) {
            .tSize = {
                .iHeight = tRegion.tSize.iHeight - c_tileWhiteDotAlphaQ4.tRegion.tSize.iHeight * 2,
                .iWidth = tRegion.tSize.iWidth,
            },
            .tLocation = {
                .iX = ptRegion->tLocation.iX,
                .iY = ptRegion->tLocation.iY + c_tileWhiteDotAlphaQ4.tRegion.tSize.iHeight,
            },
        }, 
        (__arm_2d_color_t){tColour},
        hwFillAlpha);

    arm_2d_op_wait_async(NULL);
                            
    //! copy the bottom right corner 
    tRegion.tLocation.iY += ptRegion->tSize.iHeight - c_tileWhiteDotAlphaQ4.tRegion.tSize.iHeight;

    arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tRegion, 
                                            &c_tileWhiteDotAlphaQ4, 
                                            (__arm_2d_color_t){tColour},
                                            chOpacity);

    arm_2d_op_wait_async(NULL);

    //! copy the bottom left corner 
    tRegion.tLocation.iX = ptRegion->tLocation.iX;
                            
    arm_2d_fill_colour_with_mask_and_opacity(  
                                            ptTarget, 
                                            &tRegion, 
                                            &c_tileWhiteDotAlphaQ3, 
                                            (__arm_2d_color_t){tColour},
                                            chOpacity);

    arm_2d_op_wait_async(NULL);

    arm_2dp_fill_colour_with_opacity(   
        NULL,
        ptTarget, 
        &(arm_2d_region_t) {
            .tSize = {
                .iHeight = c_tileWhiteDotAlphaQ4.tRegion.tSize.iHeight,
                .iWidth = tRegion.tSize.iWidth - c_tileWhiteDotAlphaQ4.tRegion.tSize.iWidth * 2,
            },
            .tLocation = {
                .iX = tRegion.tLocation.iX + c_tileWhiteDotAlphaQ4.tRegion.tSize.iWidth,
                .iY = tRegion.tLocation.iY,
            },
        }, 
        (__arm_2d_color_t){tColour},
        hwFillAlpha);

    arm_2d_op_wait_async(NULL);

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
