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

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

dcl_fb(s_tCorner)
impl_fb(s_tCorner, 7, 7, uint8_t,
    .tInfo = {
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {
            .chScheme = ARM_2D_COLOUR_8BIT,
        },
    },
);


const arm_2d_tile_t c_tileWhiteDotAlphaQ4 = {
    .tRegion = {
        .tLocation = {
            .iX = 7,
            .iY = 7,
        },
        .tSize = {
            .iWidth = 7,
            .iHeight = 7,
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
    .ptParent = (arm_2d_tile_t *)&c_tileWhiteDotMask,
};

const arm_2d_tile_t c_tileWhiteDotAlphaQ1 = {
    .tRegion = {
        .tLocation = {
            .iX = 7,
            .iY = 0,
        },
        .tSize = {
            .iWidth = 7,
            .iHeight = 7,
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
    .ptParent = (arm_2d_tile_t *)&c_tileWhiteDotMask,
};

const arm_2d_tile_t c_tileWhiteDotAlphaQ2 = {
    .tRegion = {
        .tLocation = {
            .iX = 0,
            .iY = 0,
        },
        .tSize = {
            .iWidth = 7,
            .iHeight = 7,
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
    .ptParent = (arm_2d_tile_t *)&c_tileWhiteDotMask,
};

const arm_2d_tile_t c_tileWhiteDotAlphaQ3 = {
    .tRegion = {
        .tLocation = {
            .iX = 0,
            .iY = 7,
        },
        .tSize = {
            .iWidth = 7,
            .iHeight = 7,
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
    .ptParent = (arm_2d_tile_t *)&c_tileWhiteDotMask,
};

/*============================ IMPLEMENTATION ================================*/

void draw_round_corner_box( const arm_2d_tile_t *ptTarget, 
                            const arm_2d_region_t *ptRegion,
                            COLOUR_INT tColour,
                            uint8_t chAlpha,
                            bool bIsNewFrame)
{
    assert(NULL != ptTarget);
    ARM_2D_UNUSED(bIsNewFrame);
    uint16_t hwFillAlpha = (0xFF == chAlpha) ? 0xFF : (0xFF * chAlpha) >> 8;
    
    arm_2d_region_t tTempRegion = {0};
    if (NULL == ptRegion) {
        tTempRegion.tSize = ptTarget->tRegion.tSize;
        ptRegion = (const arm_2d_region_t *)&tTempRegion;
    }

    arm_2d_region_t tRegion = *ptRegion;

#if 0
    //! copy the top left corner
    arm_2d_c8bit_tile_copy_with_xy_mirror(  &c_tileWhiteDotAlphaQuarter, 
                                            &s_tCorner, 
                                            NULL);
                            
    arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tRegion, 
                                            &s_tCorner, 
                                            (__arm_2d_color_t){tColour},
                                            chAlpha);
                                                
    arm_2d_op_wait_async(NULL);

    //! copy the top right corner
    tRegion.tLocation.iX += ptRegion->tSize.iWidth - s_tCorner.tRegion.tSize.iWidth;
    arm_2d_c8bit_tile_copy_with_y_mirror(   &c_tileWhiteDotAlphaQuarter, 
                                            &s_tCorner, 
                                            NULL);
                            
    arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tRegion, 
                                            &s_tCorner, 
                                            (__arm_2d_color_t){tColour},
                                            chAlpha);

    arm_2d_op_wait_async(NULL);

    arm_2dp_fill_colour_with_opacity(   
        NULL,
        ptTarget, 
        &(arm_2d_region_t) {
            .tSize = {
                .iHeight = s_tCorner.tRegion.tSize.iHeight,
                .iWidth = tRegion.tSize.iWidth - s_tCorner.tRegion.tSize.iWidth * 2,
            },
            .tLocation = {
                .iX = ptRegion->tLocation.iX + s_tCorner.tRegion.tSize.iWidth,
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
                .iHeight = tRegion.tSize.iHeight - s_tCorner.tRegion.tSize.iHeight * 2,
                .iWidth = tRegion.tSize.iWidth,
            },
            .tLocation = {
                .iX = ptRegion->tLocation.iX,
                .iY = ptRegion->tLocation.iY + s_tCorner.tRegion.tSize.iHeight,
            },
        }, 
        (__arm_2d_color_t){tColour},
        hwFillAlpha);

    arm_2d_op_wait_async(NULL);
                            
    //! copy the bottom right corner 
    tRegion.tLocation.iY += ptRegion->tSize.iHeight - s_tCorner.tRegion.tSize.iHeight;
    arm_2d_c8bit_tile_copy_only(&c_tileWhiteDotAlphaQuarter, 
                                &s_tCorner, 
                                NULL);

    arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tRegion, 
                                            &s_tCorner, 
                                            (__arm_2d_color_t){tColour},
                                            chAlpha);

    arm_2d_op_wait_async(NULL);

    //! copy the bottom left corner 
    tRegion.tLocation.iX = ptRegion->tLocation.iX;
    arm_2d_c8bit_tile_copy_with_x_mirror(   &c_tileWhiteDotAlphaQuarter, 
                                            &s_tCorner, 
                                            NULL);
                            
    arm_2d_fill_colour_with_mask_and_opacity(  
                                            ptTarget, 
                                            &tRegion, 
                                            &s_tCorner, 
                                            (__arm_2d_color_t){tColour},
                                            chAlpha);

    arm_2d_op_wait_async(NULL);

    arm_2dp_fill_colour_with_opacity(   
        NULL,
        ptTarget, 
        &(arm_2d_region_t) {
            .tSize = {
                .iHeight = s_tCorner.tRegion.tSize.iHeight,
                .iWidth = tRegion.tSize.iWidth - s_tCorner.tRegion.tSize.iWidth * 2,
            },
            .tLocation = {
                .iX = tRegion.tLocation.iX + s_tCorner.tRegion.tSize.iWidth,
                .iY = tRegion.tLocation.iY,
            },
        }, 
        (__arm_2d_color_t){tColour},
        hwFillAlpha);

    arm_2d_op_wait_async(NULL);
#else
    //! copy the top left corner
    arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tRegion, 
                                            &c_tileWhiteDotAlphaQ2, 
                                            (__arm_2d_color_t){tColour},
                                            chAlpha);
                                                
    arm_2d_op_wait_async(NULL);

    //! copy the top right corner
    tRegion.tLocation.iX += ptRegion->tSize.iWidth - s_tCorner.tRegion.tSize.iWidth;
                            
    arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tRegion, 
                                            &c_tileWhiteDotAlphaQ1, 
                                            (__arm_2d_color_t){tColour},
                                            chAlpha);

    arm_2d_op_wait_async(NULL);

    arm_2dp_fill_colour_with_opacity(   
        NULL,
        ptTarget, 
        &(arm_2d_region_t) {
            .tSize = {
                .iHeight = s_tCorner.tRegion.tSize.iHeight,
                .iWidth = tRegion.tSize.iWidth - s_tCorner.tRegion.tSize.iWidth * 2,
            },
            .tLocation = {
                .iX = ptRegion->tLocation.iX + s_tCorner.tRegion.tSize.iWidth,
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
                .iHeight = tRegion.tSize.iHeight - s_tCorner.tRegion.tSize.iHeight * 2,
                .iWidth = tRegion.tSize.iWidth,
            },
            .tLocation = {
                .iX = ptRegion->tLocation.iX,
                .iY = ptRegion->tLocation.iY + s_tCorner.tRegion.tSize.iHeight,
            },
        }, 
        (__arm_2d_color_t){tColour},
        hwFillAlpha);

    arm_2d_op_wait_async(NULL);
                            
    //! copy the bottom right corner 
    tRegion.tLocation.iY += ptRegion->tSize.iHeight - s_tCorner.tRegion.tSize.iHeight;

    arm_2d_fill_colour_with_mask_and_opacity(   
                                            ptTarget, 
                                            &tRegion, 
                                            &c_tileWhiteDotAlphaQ4, 
                                            (__arm_2d_color_t){tColour},
                                            chAlpha);

    arm_2d_op_wait_async(NULL);

    //! copy the bottom left corner 
    tRegion.tLocation.iX = ptRegion->tLocation.iX;
                            
    arm_2d_fill_colour_with_mask_and_opacity(  
                                            ptTarget, 
                                            &tRegion, 
                                            &c_tileWhiteDotAlphaQ3, 
                                            (__arm_2d_color_t){tColour},
                                            chAlpha);

    arm_2d_op_wait_async(NULL);

    arm_2dp_fill_colour_with_opacity(   
        NULL,
        ptTarget, 
        &(arm_2d_region_t) {
            .tSize = {
                .iHeight = s_tCorner.tRegion.tSize.iHeight,
                .iWidth = tRegion.tSize.iWidth - s_tCorner.tRegion.tSize.iWidth * 2,
            },
            .tLocation = {
                .iX = tRegion.tLocation.iX + s_tCorner.tRegion.tSize.iWidth,
                .iY = tRegion.tLocation.iY,
            },
        }, 
        (__arm_2d_color_t){tColour},
        hwFillAlpha);

    arm_2d_op_wait_async(NULL);
#endif
}


#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
