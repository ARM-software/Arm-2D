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
#include "./app_cfg.h"
#include "./progress_bar_simple.h"
#include "arm_2d.h"
#include "platform.h"
#include <math.h>

#if defined(__clang__)
#   pragma clang diagnostic push
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
extern uint32_t SystemCoreClock;

/*============================ PROTOTYPES ====================================*/
__attribute__((nothrow)) 
extern int64_t clock(void);

/*============================ LOCAL VARIABLES ===============================*/

extern const uint8_t c_bmpSmallWhiteDot[];
const arm_2d_tile_t c_tileSmallWhiteDot = {
    .tRegion = {
        .tSize = {
            .iWidth = 7,
            .iHeight = 7
        },
    },
    .tInfo.bIsRoot = true,
    .phwBuffer = (uint16_t *)c_bmpSmallWhiteDot,
};


const arm_2d_tile_t c_tileSemisphere = {
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
    .tInfo.bIsRoot = false,
    .tInfo.bDerivedResource = true,
    .ptParent = (arm_2d_tile_t *)&c_tileSmallWhiteDot,
};
/*============================ IMPLEMENTATION ================================*/


void progress_bar_simple_init(void)
{

}

void progress_bar_simple_show(const arm_2d_tile_t *ptTarget, int_fast16_t iProgress)
{
    int_fast16_t iWidth = ptTarget->tRegion.tSize.iWidth * 3 >> 3;         //!< 3/8 Width
 
    ASSERT(NULL != ptTarget);
    ASSERT(iProgress <= 1000);
 
    arm_2d_region_t tBarRegion = {
        .tLocation = {
           .iX = (ptTarget->tRegion.tSize.iWidth - (int16_t)iWidth) / 2,
           .iY = (ptTarget->tRegion.tSize.iHeight - c_tileSmallWhiteDot.tRegion.tSize.iHeight) / 2,
        },
        .tSize = {
            .iWidth = (int16_t)iWidth,
            .iHeight = c_tileSmallWhiteDot.tRegion.tSize.iHeight,
        },
    };
    
    //! draw a white box
    arm_2d_rgb16_fill_colour(ptTarget, &tBarRegion, GLCD_COLOR_WHITE);
    
    
    //! draw semispheres
    do {
        arm_2d_region_t tSemisphere = {
            .tSize = c_tileSemisphere.tRegion.tSize,
            .tLocation = {
                .iX = tBarRegion.tLocation.iX - c_tileSemisphere.tRegion.tSize.iWidth,
                .iY = tBarRegion.tLocation.iY,
            },
        };
        arm_2d_rgb16_tile_copy_with_colour_keying( 
                                &c_tileSemisphere,          //!< source tile
                                ptTarget,                   //!< display buffer
                                &tSemisphere,               //!< region to draw
                                GLCD_COLOR_BLACK,
                                ARM_2D_CP_MODE_COPY);       //!< copy only
                                       
                               
        tSemisphere.tLocation.iX = tBarRegion.tLocation.iX + tBarRegion.tSize.iWidth;
        arm_2d_rgb16_tile_copy_with_colour_keying( 
                                &c_tileSemisphere,          //!< source tile
                                ptTarget,                   //!< display buffer
                                &tSemisphere,               //!< region to draw
                                GLCD_COLOR_BLACK,
                                ARM_2D_CP_MODE_COPY |       //!< copy with x-mirroring
                                ARM_2D_CP_MODE_X_MIRROR);   
    } while(0);
    
    //! draw inner bar
    tBarRegion.tSize.iHeight-=2;
    tBarRegion.tSize.iWidth-=2;
    tBarRegion.tLocation.iX += 1;
    tBarRegion.tLocation.iY += 1;
    arm_2d_rgb16_fill_colour(ptTarget, &tBarRegion, GLCD_COLOR_BLACK);
    
    
    //! calculate the width of the inner stripe 
    tBarRegion.tSize.iWidth = tBarRegion.tSize.iWidth * (int16_t)iProgress / 1000;
    
    //! draw the inner stripe
    arm_2d_rgb16_fill_colour(ptTarget, &tBarRegion, GLCD_COLOR_WHITE);
    
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif
