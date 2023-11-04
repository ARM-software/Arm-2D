/*
 * Copyright (c) 2009-2022 Arm Limited. All rights reserved.
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
#include "./arm_extra_controls.h"
#include "./__common.h"
#include "arm_2d_helper.h"
#include "arm_2d.h"

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
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileSmallDotMask;

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/


const arm_2d_tile_t c_tileSemisphereLeft = {
    .tRegion = {
        .tLocation = {
            .iX = 0,
            .iY = 0,
        },
        .tSize = {
            .iWidth = 4,
            .iHeight = 7
        },
    },
    .tInfo = {
        .bIsRoot = false,
        .bHasEnforcedColour = true,
        .bDerivedResource = true,
        .tColourInfo = {
            .chScheme = ARM_2D_COLOUR_8BIT,
        },
    },
    .ptParent = (arm_2d_tile_t *)&c_tileSmallDotMask,
};

const arm_2d_tile_t c_tileSemisphereRight = {
    .tRegion = {
        .tLocation = {
            .iX = 3,
            .iY = 0,
        },
        .tSize = {
            .iWidth = 4,
            .iHeight = 7
        },
    },
    .tInfo = {
        .bIsRoot = false,
        .bHasEnforcedColour = true,
        .bDerivedResource = true,
        .tColourInfo = {
            .chScheme = ARM_2D_COLOUR_8BIT,
        },
    },
    .ptParent = (arm_2d_tile_t *)&c_tileSmallDotMask,
};
/*============================ IMPLEMENTATION ================================*/


void progress_bar_simple_init(void)
{

}

void __progress_bar_simple_show(const arm_2d_tile_t *ptTarget, 
                                const arm_2d_region_t *ptRegion,
                                int_fast16_t iProgress)
{
    int_fast16_t iWidth = 0;
    arm_2d_region_t tTargetRegion = {.tSize = ptTarget->tRegion.tSize,};
    if (NULL == ptRegion) {
        ptRegion = &tTargetRegion;
        iWidth = ptRegion->tSize.iWidth * 3 >> 3;         //!< 3/8 Width
    } else {
        iWidth = ptRegion->tSize.iWidth;
    }

    arm_2d_region_t tBarRegion = {
        .tLocation = {
           .iX = ptRegion->tLocation.iX + (ptRegion->tSize.iWidth - (int16_t)iWidth) / 2,
           .iY = ptRegion->tLocation.iY + (ptRegion->tSize.iHeight - c_tileSmallDotMask.tRegion.tSize.iHeight) / (int16_t)2,
        },
        .tSize = {
            .iWidth = (int16_t)iWidth,
            .iHeight = c_tileSmallDotMask.tRegion.tSize.iHeight,
        },
    };

    
    // draw a white box
    arm_2d_helper_draw_box(ptTarget, &tBarRegion, 1, GLCD_COLOR_WHITE, 255 - 64);
    
    arm_2d_op_wait_async(NULL);
    // draw semispheres
    do {
        arm_2d_region_t tSemisphere = {
            .tSize = c_tileSemisphereLeft.tRegion.tSize,
            .tLocation = {
                .iX = tBarRegion.tLocation.iX - c_tileSemisphereLeft.tRegion.tSize.iWidth,
                .iY = tBarRegion.tLocation.iY,
            },
        };

        arm_2d_fill_colour_with_mask_and_opacity(   
                                        ptTarget,
                                        &tSemisphere,
                                        &c_tileSemisphereLeft,
                                        (__arm_2d_color_t) {GLCD_COLOR_WHITE},
                                        255 - 64);

        arm_2d_op_wait_async(NULL);

        tSemisphere.tLocation.iX = tBarRegion.tLocation.iX + tBarRegion.tSize.iWidth;


        arm_2d_fill_colour_with_mask_and_opacity(
                                        ptTarget,
                                        &tSemisphere,
                                        &c_tileSemisphereRight,
                                        (__arm_2d_color_t) {GLCD_COLOR_WHITE},
                                        255 - 64);
        arm_2d_op_wait_async(NULL);
    } while(0);
    
    // draw inner bar
    tBarRegion.tSize.iHeight-=2;
    tBarRegion.tSize.iWidth-=2;
    tBarRegion.tLocation.iX += 1;
    tBarRegion.tLocation.iY += 1;

    arm_2d_fill_colour_with_opacity(ptTarget, 
                                    &tBarRegion, 
                                    (__arm_2d_color_t) {GLCD_COLOR_BLACK},
                                    64);
    
    arm_2d_op_wait_async(NULL);
    // calculate the width of the inner stripe 
    tBarRegion.tSize.iWidth = tBarRegion.tSize.iWidth * (int16_t)iProgress / 1000;
    
    // draw the inner stripe
    arm_2d_fill_colour_with_opacity(ptTarget, 
                                &tBarRegion, 
                                (__arm_2d_color_t) {GLCD_COLOR_WHITE},
                                255 - 64);
    arm_2d_op_wait_async(NULL);
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
