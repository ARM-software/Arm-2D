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
#include "./progress_bar_drill.h"
#include "./__common.h"
#include "arm_2d.h"

#include <assert.h>

#include "arm_2d_helper.h"

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
#   pragma clang diagnostic ignored "-Wgnu-statement-expression"
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#   pragma GCC diagnostic ignored "-Wunused-value"
#elif __IS_COMPILER_IAR__
#   pragma diag_suppress=Pa089,Pe188,Pe174
#endif

/*============================ MACROS ========================================*/

#ifndef PROGRESS_BAR_DRILL_SPEED
#   define PROGRESS_BAR_DRILL_SPEED     10
#endif

#if __GLCD_CFG_COLOUR_DEPTH__ == 8

#   define c_tileBlueSlash           c_tileBlueSlashGRAY8

#elif __GLCD_CFG_COLOUR_DEPTH__ == 16

#   define c_tileBlueSlash           c_tileBlueSlashRGB565

#elif __GLCD_CFG_COLOUR_DEPTH__ == 32

#   define c_tileBlueSlash           c_tileBlueSlashCCCA8888
#else
#   error Unsupported colour depth!
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const arm_2d_tile_t c_tileBlueSlash;
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

ARM_NOINIT static int64_t s_lLastTime;
ARM_NOINIT static uint32_t s_wUnit;

/*============================ IMPLEMENTATION ================================*/


void progress_bar_drill_init(void)
{
    s_lLastTime = arm_2d_helper_get_system_timestamp();
    s_wUnit = PROGRESS_BAR_DRILL_SPEED;
}

void __progress_bar_drill_show( const arm_2d_tile_t *ptTarget,
                                const arm_2d_region_t *ptRegion,
                                int_fast16_t iProgress,
                                COLOUR_INT tBarColour,
                                bool bIsNewFrame)
{
    static uint8_t s_chOffset = 0;

    assert(NULL != ptTarget);

    if (-1 == (intptr_t)ptTarget) {
        ptTarget = arm_2d_get_default_frame_buffer();
    }

    if (iProgress > 1000) {
        iProgress = 0;
    }

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
           .iY = ptRegion->tLocation.iY + (ptRegion->tSize.iHeight - c_tileBlueSlash.tRegion.tSize.iHeight) / (int16_t)2,
        },
        .tSize = {
            .iWidth = (int16_t)iWidth,
            .iHeight = c_tileBlueSlash.tRegion.tSize.iHeight,
        },
    };

    //! update offset
    if (bIsNewFrame) {
        if (arm_2d_helper_is_time_out(s_wUnit, &s_lLastTime)) {
//            int64_t lClocks = arm_2d_helper_get_system_timestamp();
//            int32_t nElapsed = (int32_t)((lClocks - s_lLastTime));
//            
//            if (nElapsed >= (int32_t)s_wUnit) {
//                s_lLastTime = lClocks;
            s_chOffset++;
            if (s_chOffset >= c_tileBlueSlash.tRegion.tSize.iWidth) {
                s_chOffset = 0;
            }
//            }
        }
    }

    if (!arm_2d_helper_pfb_is_region_being_drawing(ptTarget, &tBarRegion, NULL)) {
        return ;
    }

    //! draw a white box
    arm_2d_fill_colour(ptTarget, &tBarRegion, __RGB(0xa5, 0xc6, 0xef));
    
    arm_2d_op_wait_async(NULL);
    
    //! pave inter texture
    tBarRegion.tSize.iHeight-=2;
    tBarRegion.tSize.iWidth-=2;
    tBarRegion.tLocation.iX += 1;
    tBarRegion.tLocation.iY += 1;

    do {
        
        arm_2d_region_t tInnerRegion = {
            .tSize = {
                .iWidth = tBarRegion.tSize.iWidth + c_tileBlueSlash.tRegion.tSize.iWidth,
                .iHeight = tBarRegion.tSize.iHeight,
            },
            .tLocation = {
                .iX = -c_tileBlueSlash.tRegion.tSize.iWidth + s_chOffset,
            },
        };
        arm_2d_tile_t tileInnerSlot;
    
        //! generate a child tile for texture paving
        arm_2d_tile_generate_child(ptTarget, &tBarRegion, &tileInnerSlot, false);
    
        
        arm_2d_tile_fill_only( &c_tileBlueSlash, 
                            &tileInnerSlot, 
                            &tInnerRegion);

        arm_2d_op_wait_async(NULL);

    } while(0);

    
    if (iProgress > 0) {
        //! calculate the width of the inner stripe 
        tBarRegion.tSize.iWidth = tBarRegion.tSize.iWidth * (int16_t)iProgress / 1000;
        
        //! draw the inner stripe
        arm_2d_fill_colour(ptTarget, &tBarRegion, tBarColour);
        arm_2d_op_wait_async(NULL);
    }
    
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif
